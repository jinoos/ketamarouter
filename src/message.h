#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "str.h"

//
// type of data
//
// Request
#define MSG_CMD_HEADER      '+'
#define MSG_CMD_HEADER_STR  "+"
// String
#define MSG_STR_HEADER      '$'
#define MSG_STR_HEADER_STR  "$"
// Key,Value
#define MSG_KV_HEADER       '%'
#define MSG_KV_HEADER_STR   "%"
// Key,Value seperator
#define MSG_KV_SEPT         '='
#define MSG_KV_SEPT_STR     "="

#define MSG_CRLF            "\r\n"

typedef struct data_t
{
    char            *data;
    struct data_t   *next;
} data_t;

typedef struct cmd_t
{
    str_t       *name;
    uint16_t    param_count;
    char        *data;
} cmd_t;

typedef str_t param_str_t;

typedef struct param_kv_t
{
    str_t      *key;
    str_t      *value;
} param_kv_t;

typedef enum param_type
{
    PARAM_TYPE_STR = 0,
    PARAM_TYPE_KV = 1
} param_type;

typedef struct param_t
{
    param_type  type;
    param_str_t *str;
    param_kv_t  *kv;
    struct param_t  *next;
    char        *data;
} param_t;

typedef struct msg_t msg_t;

typedef struct msg_buf_t
{
    pabuf_t     *msg;
    pabuf_t     *str;
    pabuf_t     *cmd;
    pabuf_t     *param;
    pabuf_t     *param_kv;
    pabuf_t     *data;
} msg_buf_t;

enum msg_stat
{
    MSG_STAT_NONE       = 0,
    MSG_STAT_DATA_MORE  = 1,
    MSG_STAT_DONE       = 9
};

struct msg_t
{
    msg_buf_t   *buf;
    cmd_t       *cmd;
    uint16_t    param_count;

    param_t     *param_first;
    param_t     *param_last;

    enum msg_stat           stat;
};

msg_buf_t* msg_buf_init();

msg_t *msg_init(msg_buf_t *buf);

void msg_clean(msg_t *msg);

cmd_t* msg_init_cmd(msg_buf_t *buf, char *cmd, size_t cmd_len, uint16_t param_count, char *data);
cmd_t* msg_gen_cmd(msg_buf_t *buf, const char *cmd, const size_t cmd_len, const uint16_t param_count);
cmd_t* msg_set_cmd(msg_t *msg, cmd_t *cmd);

param_t* msg_init_param_str(msg_buf_t *buf, char *str, size_t str_len, char *data);
param_t* msg_init_param_kv(msg_buf_t *buf, char *key, size_t key_len, char *value, size_t value_len, char *data);
param_t* msg_gen_param_str(msg_buf_t *buf, const char *str, const size_t str_len);
param_t* msg_gen_param_kv(msg_buf_t *buf, const char *key, const size_t key_len, const char *value, const size_t value_len);
uint16_t msg_add_param(msg_t *msg, param_t *param);

uint16_t msg_get_param_count(msg_t *msg);

enum msg_stat msg_get_stat(msg_t *msg);

void msg_free(msg_t *msg);
void msg_print(msg_t *msg);

#endif // _MESSAGE_H_
