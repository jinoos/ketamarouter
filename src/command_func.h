#ifndef _COMMAND_FUNC_H_
#define _COMMAND_FUNC_H_

#include "client.h"
#include "message.h"

typedef struct client_t client_t;

enum cmd_func_ret
{
    CMD_FUNC_DONE,
    CMD_FUNC_UNKNOWN,
    CMD_FUNC_ERR_PARAM_TYPE,
    CMD_FUNC_ZERO_NODES,
    CMD_FUNC_ERR_NUKNOWN
};

enum cmd_func_ret command_GET(client_t *client, msg_t *msg);
enum cmd_func_ret command_PING(client_t *client, msg_t *msg);


#endif // _COMMAND_FUNC_H_
