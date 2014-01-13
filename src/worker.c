#include <event2/event.h>

#include "worker.h"
#include "message.h"
#include "logging.h"

worker_t* worker_init(server_t *s)
{
    worker_t *w = calloc(sizeof(worker_t), 1);
    w->server = s;

    w->evbase = event_base_new();
    event_base_add_virtual(w->evbase);
    pthread_mutex_init(&w->lock, NULL);
    w->msg_buf = msg_buf_init();

    return w;
}

struct bufferevent* worker_add_new_connection(worker_t *w, evutil_socket_t c_sock)
{
    struct bufferevent *be = bufferevent_socket_new(w->evbase, c_sock, BEV_OPT_CLOSE_ON_FREE);
    worker_increase_count(w);
    return be;
}

uint32_t worker_get_count(worker_t *w)
{
    return w->num_client;
}

void worker_increase_count(worker_t *w)
{
    pthread_mutex_lock(&w->lock);
    w->num_client++;
    pthread_mutex_unlock(&w->lock);
}

void worker_decrease_count(worker_t *w)
{
    pthread_mutex_lock(&w->lock);
    w->num_client--;
    pthread_mutex_unlock(&w->lock);
}

void worker_start(worker_t *w)
{
    event_base_dispatch(w->evbase);
}

void worker_increase_req_ok(worker_t *w)
{
    w->req_all++;
    w->req_ok++;
}

void worker_increase_req_err(worker_t *w)
{
    w->req_all++;
    w->req_err++;
}
