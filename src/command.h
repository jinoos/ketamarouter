#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdint.h>
#include <glib.h>
#include "str.h"
#include "message.h"
#include "client.h"
#include "command_func.h"

typedef struct client_t client_t;

typedef struct command_t
{
    str_t       *name;
    uint16_t    param_min;      // it means number of mandatory parameters
    uint16_t    param_max;      // parameter limitation

    enum cmd_func_ret (*func)(client_t*, msg_t*);
} command_t;

typedef GHashTable command_list_t;

#define COMMAND_STR_GET     "GET"
#define COMMAND_STR_ERR     "ERROR"
#define COMMAND_STR_NODE    "NODE"
#define COMMAND_STR_PONG    "PONG"


command_list_t* command_list_init();
command_t* command_list_getl(command_list_t *cl, char *name, size_t len);
command_t* command_list_get(command_list_t *cl, char *name);
void command_list_insert(command_list_t *cl, char *name, uint16_t param_min, uint16_t prarm_max, enum cmd_func_ret (*func)(client_t*, msg_t*));

command_t* command_init(char *name, uint16_t param_min, uint16_t prarm_max, enum cmd_func_ret (*func)(client_t*, msg_t*));
void command_free(command_t *cmd);
command_list_t* command_list_load();

enum cmd_func_ret command_proc(client_t *client, msg_t *msg);

#endif // _COMMAND_H_
