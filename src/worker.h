#ifndef _WORKER_H_
#define _WORKER_H_

#include "server.h"

typedef struct server_t server_t;
typedef struct worker_t
{
    pthread_mutex_t     lock;
    server_t            *server;

    uint32_t            num_client;
    struct event_base   *evbase;

    uint64_t                req_all;
    uint64_t                req_ok;
    uint64_t                req_err;

    msg_buf_t               *msg_buf;
} worker_t;

worker_t* worker_init(server_t *s);
struct bufferevent* worker_add_new_connection(worker_t *w, evutil_socket_t c_sock);
uint32_t worker_get_count(worker_t *w);
void worker_increase_count(worker_t *w);
void worker_decrease_count(worker_t *w);
void worker_start(worker_t *w);
void worker_increase_req_ok(worker_t *w);
void worker_increase_req_err(worker_t *w);

#endif // _WORKER_H_
