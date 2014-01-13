#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "logging.h"
#include "command_func.h"
#include "response.h"

/// typedef UINT16_MAX COMMAND_PARAM_MAX_COUNT;

command_list_t* command_list_init()
{
    return g_hash_table_new(str_hash, str_equal);
}

command_t* command_list_getl(command_list_t *cl, char *name, size_t len)
{
    str_t *key = str_init(name, len);
    command_t *cmd = g_hash_table_lookup(cl, key);
    str_free(key);
    return cmd;
}

command_t* command_list_get(command_list_t *cl, char *name)
{
    return command_list_getl(cl, name, strlen(name));
}

void command_list_insert(command_list_t *cl, char *name, uint16_t param_min, uint16_t param_max, enum cmd_func_ret (*func)(client_t*, msg_t*))
{
    command_t *cmd = command_list_get(cl, name);
    if(cmd != NULL)
    {
        g_hash_table_remove(cl, cmd->name);
        command_free(cmd);
    }
    cmd = command_init(name, param_min, param_max, func);
    g_hash_table_insert(cl, cmd->name, cmd);
}

command_t* command_init(char *name, uint16_t param_min, uint16_t param_max, enum cmd_func_ret (*func)(client_t*, msg_t*))
{
    command_t *cmd = malloc(sizeof(command_t));
    cmd->name = str_init_alloc(name, strlen(name));
    cmd->param_min = param_min;
    cmd->param_max = param_max;
    cmd->func = func;
    return cmd;
}

void command_free(command_t *cmd)
{
    str_free(cmd->name);
    free(cmd);
}

command_list_t* command_list_load()
{
    command_list_t *cl = command_list_init();

    command_t *cmd;
    command_list_insert(cl, "GET", 1, 1, command_GET);
    command_list_insert(cl, "PING", 0, 0, command_PING);

    return cl;
}

enum cmd_func_ret command_proc(client_t *c, msg_t *m)
{
    command_t *cmd = command_list_getl(c->server->command_list, m->cmd->name->data, m->cmd->name->len);
    if(cmd == NULL)
    {
        resp_error_unknown_command(c->server->msg_buf, c->socket);
        return CMD_FUNC_UNKNOWN;
    }

    return cmd->func(c, m);
}
