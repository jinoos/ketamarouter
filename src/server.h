#ifndef _SERVER_H_
#define _SERVER_H_

#include <glib.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "command.h"
#include "message.h"
#include "ketama.h"
#include "setting.h"

typedef GHashTable command_list_t;

enum server_stat
{
    SERVER_STAT_STARTING,
    SERVER_STAT_READY,
    SERVER_STAT_RUNNING,
    SERVER_STAT_SUSPENDING,
    SERVER_STAT_SUSPENDED,
    SERVER_STAT_RESUMING,
    SERVER_STAT_SHUTDOWNING,
    SERVER_STAT_SHUTDOWN
};

typedef struct worker_t worker_t;

typedef struct server_t
{
    evutil_socket_t         sock;
    struct sockaddr_in      addr;

    struct event_base       *evbase;
    struct event_base       *evsignal;
    struct evconnlistener   *evlistener;

	uint64_t				conn_all_count;
	uint64_t				conn_cur_count;
	uint64_t				conn_max;

    uint64_t                req_all;
    uint64_t                req_ok;
    uint64_t                req_err;

    command_list_t          *command_list;

    msg_buf_t               *msg_buf;

    Ketama                  *ketama;

    setting_t               *setting;

    size_t                  worker_count;
    worker_t                **worker_list;
    pthread_t               **worker_thread;

    enum server_stat        stat;
} server_t;

struct sockaddr_in* server_addr_init(struct sockaddr_in *addr, int port);
server_t* server_init(int port, size_t num_of_workers);
int server_start(server_t *server);
void server_event_accept(struct evconnlistener *listener,
                            evutil_socket_t c_socket,
                            struct sockaddr *c_addr,
                            int socklen, void *arg);

void server_event_error(struct evconnlistener *listener, void *arg);
void client_event_error(struct bufferevent *bev, short events, void *arg);
void client_event_read(struct bufferevent *bev, void *arg);
void client_event_write(struct bufferevent *bev, void *arg);

worker_t* server_get_least_connection_worker(worker_t **wl, size_t wcnt);

#endif // _SERVER_H_
