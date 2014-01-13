#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "response.h"
#include "logging.h"
#include "command.h"
#include "error_str.h"

enum RESP_RES resp_msg(evutil_socket_t sock, msg_t *msg)
{
    char buf[RESP_WRITE_BUFFER];
    int res;

    if(msg->stat != MSG_STAT_DONE)
    {
        return RESP_RES_MSG_ERR;
    }

    log_debug("send message (cmd:%.*s) to client socket:%d", msg->cmd->name->len, msg->cmd->name->data, sock);
    if(log_debug_enable())
        msg_print(msg);

    res = write(sock, msg->cmd->data, strlen(msg->cmd->data));

    if(res < 0)
        return RESP_RES_SOCK_ERR;

    param_t *nparam, *param = msg->param_first;

    while(param != NULL)
    {
        nparam = param->next;
        res = write(sock, param->data, strlen(param->data));
        if(res < 0)
            return RESP_RES_SOCK_ERR;
        param = nparam;
    }
    
    return RESP_RES_OK;
}

enum RESP_RES resp_error(msg_buf_t *buf, evutil_socket_t sock, const char *errno, const char *error)
{
    msg_t *msg;
    cmd_t *cmd;
    param_t *param;
    enum RESP_RES res;

    msg = msg_init(buf);
    cmd = msg_gen_cmd(buf, COMMAND_STR_ERR, strlen(COMMAND_STR_ERR), 2);
    msg_set_cmd(msg, cmd);
    param = msg_gen_param_str(buf, errno, strlen(errno));
    msg_add_param(msg, param);
    param = msg_gen_param_str(buf, error, strlen(error));
    msg_add_param(msg, param);

    res = resp_msg(sock, msg);

    msg_free(msg);

    return res;
}

enum RESP_RES resp_node(msg_buf_t *buf, evutil_socket_t sock, const char *node)
{
    msg_t *msg;
    cmd_t *cmd;
    param_t *param;
    enum RESP_RES res;

    msg = msg_init(buf);
    cmd = msg_gen_cmd(buf, COMMAND_STR_NODE, strlen(COMMAND_STR_NODE), 1);
    msg_set_cmd(msg, cmd);
    param = msg_gen_param_str(buf, node, strlen(node));
    msg_add_param(msg, param);

    res = resp_msg(sock, msg);

    msg_free(msg);

    return res;
}

enum RESP_RES resp_pong(msg_buf_t *buf, evutil_socket_t sock)
{
    msg_t *msg;
    cmd_t *cmd;
    enum RESP_RES res;

    msg = msg_init(buf);
    cmd =  msg_gen_cmd(buf, COMMAND_STR_PONG, strlen(COMMAND_STR_PONG), 0);
    msg_set_cmd(msg, cmd);
    res = resp_msg(sock, msg);
    msg_free(msg);
    return res;
}

enum RESP_RES resp_error_unknown_command(msg_buf_t *buf, evutil_socket_t sock)
{
    return resp_error(buf, sock, ERR_UNKNOWN_COMMAND_NO, ERR_UNKNOWN_COMMAND);
}

enum RESP_RES resp_error_wrong_msg_format(msg_buf_t *buf, evutil_socket_t sock)
{
    return resp_error(buf, sock, ERR_WRONG_MSG_FORMAT_NO, ERR_WRONG_MSG_FORMAT);
}

enum RESP_RES resp_error_param_format(msg_buf_t *buf, evutil_socket_t sock)
{
    return resp_error(buf, sock, ERR_WRONG_PARAM_FORMAT_NO, ERR_WRONG_PARAM_FORMAT);
}

enum RESP_RES resp_error_max_connection(msg_buf_t *buf, evutil_socket_t sock)
{
    return resp_error(buf, sock, ERR_REACHED_MAX_CONN_NO, ERR_REACHED_MAX_CONN);
}
