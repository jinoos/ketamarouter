#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <event2/event.h>
#include "message.h"

#define RESP_WRITE_BUFFER   4096

enum RESP_RES
{
    RESP_RES_OK,
    RESP_RES_SOCK_ERR,
    RESP_RES_MSG_ERR
};

enum RESP_RES resp_msg(evutil_socket_t sock, msg_t *msg);
enum RESP_RES resp_error(msg_buf_t *buf, evutil_socket_t sock, const char *eno, const char *err);
enum RESP_RES resp_node(msg_buf_t *buf, evutil_socket_t sock, const char *node);
enum RESP_RES resp_pong(msg_buf_t *buf, evutil_socket_t sock);

enum RESP_RES resp_error_unknown_command(msg_buf_t *buf, evutil_socket_t sock);
enum RESP_RES resp_error_wrong_msg_format(msg_buf_t *buf, evutil_socket_t sock);
enum RESP_RES resp_error_param_format(msg_buf_t *buf, evutil_socket_t sock);
enum RESP_RES resp_error_max_connection(msg_buf_t *buf, evutil_socket_t sock);

#endif // _RESPONSE_H_
