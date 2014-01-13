#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <event2/event.h>

#include "server.h"
#include "message.h"
#include "worker.h"

typedef struct server_t server_t;
typedef struct worker_t worker_t;

typedef struct client_t
{
    server_t            *server;
    worker_t            *worker;

    evutil_socket_t     socket;
    struct sockaddr_in  addr;

    struct bufferevent  *bufevent;
    struct evbuffer     *evbuf;

    msg_t               *msg;

    uint64_t            req_all;
    uint64_t            req_ok;
    uint64_t            req_err;

    time_t              last_time;
} client_t;

client_t* client_init(worker_t *w, evutil_socket_t c_sock, struct sockaddr_in *c_addr);
void client_free(client_t *client);
void client_increase_req_ok(client_t *c);
void client_increase_req_err(client_t *c);

#endif // _CLIENT_H_
