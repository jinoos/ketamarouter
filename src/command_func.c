#include "command_func.h"
#include "logging.h"
#include "ketama.h"

enum cmd_func_ret command_GET(client_t *client, msg_t *msg)
{
    Ketama *ketama = client->server->ketama;
    param_t *param = msg->param_first;
    if(param->type != PARAM_TYPE_STR)
        return CMD_FUNC_ERR_PARAM_TYPE;

    if(Ketama_get_server_num(ketama) <= 0)
        return CMD_FUNC_ZERO_NODES;

    int ordinal = Ketama_get_server_ordinal(ketama, param->str->data, param->str->len);
    char *server_addr = Ketama_get_server_address(ketama, ordinal);

    log_debug("GET command, key:%.*s, server_addr:%s", param->str->len, param->str->data, server_addr);

    resp_node(client->worker->msg_buf, client->socket, (const char*) server_addr);

    return CMD_FUNC_DONE;
}

enum cmd_func_ret command_PING(client_t *client, msg_t *msg)
{
    param_t *param = msg->param_first;
    log_debug("PING command");
    resp_pong(client->worker->msg_buf, client->socket);
    return CMD_FUNC_DONE;
}
