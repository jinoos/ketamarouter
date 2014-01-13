#include <stdlib.h>

#include "message.h"
#include "command.h"
#include "logging.h"

msg_buf_t* msg_buf_init()
{
    msg_buf_t *buf = malloc(sizeof(msg_buf_t));
    buf->msg = pabuf_init(sizeof(msg_t));
    buf->str = pabuf_init(sizeof(str_t));
    buf->cmd = pabuf_init(sizeof(cmd_t));
    buf->param = pabuf_init(sizeof(param_t));
    buf->param_kv = pabuf_init(sizeof(param_kv_t));
    buf->data = pabuf_init(sizeof(data_t));

    return buf;
}

msg_t *msg_init(msg_buf_t *buf)
{
    msg_t *msg = (msg_t*) pabuf_get(buf->msg);
    
    msg->buf = buf;
    msg->cmd = NULL;
    msg->param_count = 0;
    msg->param_first = msg->param_last = NULL;
    msg->stat = MSG_STAT_NONE;
    return msg;
}

void msg_clean(msg_t *msg)
{
    if(msg->param_count > 0)
    {
        param_t *param_next, *param = msg->param_first;
        while(param != NULL)
        {
            param_next = param->next;
            if(param->type == PARAM_TYPE_STR)
            {
                bstr_free(msg->buf->str, param->str);
            }else
            {
                bstr_free(msg->buf->str, param->kv->key);
                bstr_free(msg->buf->str, param->kv->value);
                pabuf_back(msg->buf->param_kv, param->kv);
            }
            free(param->data);
            pabuf_back(msg->buf->param, param);
            param = param_next;
        }
    }

    if(msg->cmd != NULL)
    {
        if(msg->cmd->name != NULL)
            bstr_free(msg->buf->str, msg->cmd->name);
        free(msg->cmd->data);
        pabuf_back(msg->buf->cmd, msg->cmd);
    }

    msg->cmd = NULL;
    msg->param_count = 0;
    msg->param_first = msg->param_last = NULL;
    msg->stat = MSG_STAT_NONE;

    return;
}

param_t* msg_put_paramlist(msg_t *msg, param_t *param)
{
    param->next = NULL;
    if(msg->param_first == NULL)
    {
        msg->param_first = msg->param_last = param;
    }else
    {
        msg->param_last->next = param;
        msg->param_last = param;
    }
    msg->param_count++;
    return param;
}

cmd_t* msg_init_cmd(msg_buf_t *buf, char *cmd, size_t cmd_len, uint16_t param_count, char *data)
{
    cmd_t *ncmd = pabuf_get(buf->cmd);
    ncmd->name = bstr_init(buf->str, cmd, cmd_len);
    ncmd->param_count = param_count;
    ncmd->data = data;
    return ncmd;
}

cmd_t* msg_gen_cmd(msg_buf_t *buf, const char *cmd, const size_t cmd_len, const uint16_t param_count)
{
    char *data = (char *)calloc(cmd_len + 10, 1);
    if(param_count == 0)
    {
        sprintf(data, "%c%.*s%s", MSG_CMD_HEADER, (uint32_t)cmd_len, cmd, MSG_CRLF);
    }else
    {
        sprintf(data, "%c%.*s %d%s", MSG_CMD_HEADER, (uint32_t)cmd_len, cmd, param_count, MSG_CRLF);
    }

    cmd_t *ncmd = pabuf_get(buf->cmd);
    ncmd->name = bstr_init(buf->str, data+1, cmd_len);
    ncmd->param_count = param_count;
    ncmd->data = data;
    return ncmd;
}

cmd_t* msg_set_cmd(msg_t *msg, cmd_t *cmd)
{
    msg->cmd = cmd;
        
    if(cmd->param_count > 0)
        msg->stat = MSG_STAT_DATA_MORE;
    else
        msg->stat = MSG_STAT_DONE;

    return cmd;
}

param_t* msg_init_param_str(msg_buf_t *buf, char *param, size_t param_len, char *data)
{
    param_t *nparam = pabuf_get(buf->param);
    nparam->type = PARAM_TYPE_STR;
    nparam->str = bstr_init(buf->str, param, param_len);
    nparam->kv = NULL;
    nparam->next = NULL;
    nparam->data = data;
    return nparam;
}

param_t* msg_init_param_kv(msg_buf_t *buf, char *key, size_t key_len, char *value, size_t value_len, char *data)
{
    param_t *nparam = pabuf_get(buf->param);
    nparam->type = PARAM_TYPE_KV;
    nparam->kv = pabuf_get(buf->param_kv);
    nparam->kv->key = bstr_init(buf->str, key, key_len);
    nparam->kv->value = bstr_init(buf->str, value, value_len);
    nparam->str = NULL;
    nparam->next = NULL;
    nparam->data = data;
    return nparam;
}

param_t* msg_gen_param_str(msg_buf_t *buf, const char *str, const size_t str_len)
{
    char *data = (char *)calloc(str_len + 4, 1);
    sprintf(data, "%c%.*s%s", MSG_STR_HEADER, (uint32_t)str_len, str, MSG_CRLF);

    param_t *nparam = pabuf_get(buf->param);
    nparam->type = PARAM_TYPE_STR;
    nparam->str = bstr_init(buf->str, data+1, str_len);
    nparam->kv = NULL;
    nparam->next = NULL;
    nparam->data = data;
    return nparam;
}

param_t* msg_gen_param_kv(msg_buf_t *buf, const char *key, const size_t key_len, const char *value, const size_t value_len)
{
    char *data = (char *)calloc(key_len + value_len + 5, 1);
    sprintf(data, "%c%.*s%c%.*s%s", MSG_KV_HEADER, (uint32_t)key_len, key, MSG_KV_SEPT, (uint32_t)value_len, value, MSG_CRLF);

    param_t *nparam = pabuf_get(buf->param);
    nparam->type = PARAM_TYPE_KV;
    nparam->kv = pabuf_get(buf->param_kv);
    nparam->kv->key = bstr_init(buf->str, data+1, key_len);
    nparam->kv->value = bstr_init(buf->str, data+1+key_len+1, value_len);
    nparam->str = NULL;
    nparam->next = NULL;
    nparam->data = data;
    return nparam;
}

uint16_t msg_add_param(msg_t *msg, param_t *param)
{
    msg_put_paramlist(msg, param);

    if(msg->param_count >= msg->cmd->param_count)
        msg->stat = MSG_STAT_DONE;
    else
        msg->stat = MSG_STAT_DATA_MORE;

    return msg->param_count;
}

uint16_t msg_get_param_count(msg_t *msg)
{
    return msg->param_count;
}

enum msg_stat msg_get_stat(msg_t *msg)
{
    return msg->stat;
}

void msg_free(msg_t *msg)
{
    if(msg == NULL)
        return;

    msg_buf_t *buf = msg->buf;
    msg_clean(msg);
    pabuf_back(buf->msg, msg);
    return;
}

void msg_print(msg_t *msg)
{
    char *str;
    log_debug("msg_print()");
    if(msg->cmd != NULL)
    {
        log_debug(" COMMAND : %s", msg->cmd->data);
    }

    param_t *nparam, *param = msg->param_first;

    while(param != NULL)
    {
        nparam = param->next;
        log_debug(" PARAMETER : %s", param->data);
        param = nparam;
    }
}
