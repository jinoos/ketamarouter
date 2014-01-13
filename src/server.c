#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"
#include "logging.h"
#include "event_util.h"
#include "decoder.h"
#include "command.h"
#include "response.h"
#include "worker.h"

struct sockaddr_in* server_addr_init(struct sockaddr_in *addr, int port)
{
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(port);
	return addr;
}

server_t* server_init(int port, size_t num_of_workers)
{
    server_t *server = calloc(sizeof(server_t), 1);
    server_addr_init(&(server->addr), port);
    server->evbase = event_base_new();
    server->evsignal = event_base_new();

    server->worker_count = num_of_workers;
    server->worker_list = calloc(sizeof(worker_t *), server->worker_count);
    server->worker_thread = calloc(sizeof(pthread_t *), server->worker_count);
    int i;
    for(i = 0; i < server->worker_count; i++)
    {
        server->worker_list[i] = worker_init(server);
        server->worker_thread[i] = calloc(sizeof(pthread_t), 1);
    }

    server->command_list = command_list_load();
    server->msg_buf = msg_buf_init();

    server->ketama = Ketama_new();

    Ketama_add_server(server->ketama, "127.0.0.1", 6379, 100);
    Ketama_add_server(server->ketama, "127.0.0.2", 6379, 100);
    Ketama_add_server(server->ketama, "127.0.0.3", 6379, 100);

    Ketama_create_continuum(server->ketama);
    return server;
}

int server_start(server_t *server)
{
    server->evlistener = evconnlistener_new_bind(server->evbase,
                                                    server_event_accept,
                                                    (void *)server,
                                                    LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                                                    -1,
                                                    (struct sockaddr*)&(server->addr),
                                                    sizeof(struct sockaddr_in));

    if(server->evlistener == NULL)
    {
        log_err("Cannot listen server, %s", evutil_socket_error_to_string(errno));
        exit(-1);
    }

    evconnlistener_set_error_cb(server->evlistener, server_event_error);
    log_info("List port(%d)", ntohs(server->addr.sin_port));

    event_base_dispatch(server->evbase);

    return 0;
}

worker_t* server_get_least_connection_worker(worker_t **wl, size_t wcnt)
{
    int i;
    worker_t *w = wl[0];;
    for(i = 1; i < wcnt; i++)
    {
        if(w->num_client > (wl[i])->num_client)
        {
            log_debug("## found least worker %d", i);
            w = wl[i];
        }
    }
    return w;
}

uint64_t server_get_current_connections(worker_t **wl, size_t wcnt)
{
    int i;
    uint64_t cons = 0;
    worker_t *w = wl[0];
    for(i = 0; i < wcnt; i++)
    {
        cons += worker_get_count(wl[i]);
    }
    return cons;
}

uint64_t server_get_req_all(worker_t **wl, size_t wcnt)
{
    int i;
    uint64_t req_all = 0;
    worker_t *w = wl[0];
    for(i = 0; i < wcnt; i++)
    {
        w = wl[i];
        req_all += w->req_all;
    }
    return req_all;
}

uint64_t server_get_req_ok(worker_t **wl, size_t wcnt)
{
    int i;
    uint64_t req_ok = 0;
    worker_t *w = wl[0];
    for(i = 0; i < wcnt; i++)
    {
        w = wl[i];
        req_ok += w->req_ok;
    }
    return req_ok;
}

uint64_t server_get_req_err(worker_t **wl, size_t wcnt)
{
    int i;
    uint64_t req_err = 0;
    worker_t *w = wl[0];
    for(i = 0; i < wcnt; i++)
    {
        w = wl[i];
        req_err += w->req_err;
    }
    return req_err;
}

void server_event_accept(struct evconnlistener *listener,
                            evutil_socket_t c_socket,
                            struct sockaddr *c_addr,
                            int socklen, void *arg)
{
    log_info("Connected client socket:%d, from:%s:%d", c_socket,
                                                        inet_ntoa(((struct sockaddr_in*)c_addr)->sin_addr),
                                                        ntohs(((struct sockaddr_in*)c_addr)->sin_port));
    server_t *s= (server_t*) arg;

    if(setting_get_max_connections(s->setting) <= server_get_current_connections(s->worker_list, s->worker_count))
    {
        log_warn("Reached to max connection(%" PRIu64 ")", s->conn_cur_count);
        resp_error_max_connection(s->msg_buf, c_socket);
        close(c_socket);
        return;
    }

    // debug용
    if(s->conn_cur_count > s->conn_max)
    {
        s->conn_max = s->conn_cur_count;
    }

    worker_t *w = server_get_least_connection_worker(s->worker_list, s->worker_count);

    client_t *c = client_init(w, c_socket, (struct sockaddr_in*)c_addr);
    c->worker = w;
    c->bufevent = worker_add_new_connection(w, c_socket);
    c->evbuf = evbuffer_new();

    bufferevent_setcb(c->bufevent, client_event_read, client_event_write, client_event_error, c);
    bufferevent_enable(c->bufevent, EV_READ);

    log_info("Accepted client connection. socket #%d from %s:%d", c_socket, inet_ntoa(c->addr.sin_addr), ntohs(c->addr.sin_port));
    log_info("Connected clients : %"PRIu64"", server_get_current_connections(s->worker_list, s->worker_count));

    return;
}

void server_event_error(struct evconnlistener *listener, void *arg)
{
    log_debug("server_event_error");
    log_debug("Listener received error, %s", evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
}

void client_connection_release(client_t *c)
{
    log_debug("client_connection_release %p", c);
    server_t *s = c->server;
    worker_t *w = c->worker;

    if(c->socket != 0)
        evutil_closesocket(c->socket);

    worker_decrease_count(w);

    log_info("Client closed. #%d, max:%" PRIu64 ", current:%" PRIu64, c->socket, c->server->conn_max, server_get_current_connections(s->worker_list, s->worker_count));
//    log_info("Request count. current:%d all: %d", c->req_all, c->server->req_all);
    s->conn_cur_count--;
    client_free(c);

    return;
}

void client_event_error(struct bufferevent *bev, short events, void *arg)
{
    log_debug("client_event_error");
    log_debug("Client socket received error(%d)", events);

    if (events & BEV_EVENT_READING)
    {
        log_debug("- Error is BEV_EVENT_READING");
    } else if (events & BEV_EVENT_WRITING)
    {
        log_debug("- Error is BEV_EVENT_WRITING");
    } else if (events & BEV_EVENT_EOF)
    {
        log_debug("- Error is BEV_EVENT_EOF");
    } else if (events & BEV_EVENT_ERROR)
    {
        log_debug("- Error is BEV_EVENT_ERROR");
    } else if (events & BEV_EVENT_TIMEOUT)
    {
        log_debug("- Error is BEV_EVENT_TIMEOUT");
    } else if (events & BEV_EVENT_CONNECTED)
    {
        log_debug("- Error is BEV_EVENT_CONNECTED");
    }

    client_connection_release((client_t*)arg);
    return;
}

void client_event_read(struct bufferevent *bev, void *arg)
{
    log_debug("client_event_read");
    client_t *c = (client_t*) arg;
    server_t *s = c->server;
    log_debug("Received read event from client socket %d", c->socket);

    if(c->msg == NULL)
    {
        c->msg = msg_init(c->worker->msg_buf);
    }

    int res = evbuffer_add_buffer(c->evbuf, bufferevent_get_input(bev));
    char *line;

    while((line = evbuffer_readln(c->evbuf, NULL, EVBUFFER_EOL_CRLF)) != NULL)
    {
        log_debug("Read data : %s", line);
        decode_result res = decoder_run(c->msg, line);
        msg_print(c->msg);

        switch(res)
        {
            case DR_ERR_FORMAT:
                client_increase_req_err(c);
                worker_increase_req_err(c->worker);
                msg_clean(c->msg);
                log_debug("Wrong message format : %s", line);
                resp_error_wrong_msg_format(c->worker->msg_buf, c->socket);
                break;

            case DR_ERR_PARAM_FORMAT:
                client_increase_req_err(c);
                worker_increase_req_err(c->worker);
                msg_clean(c->msg);
                log_debug("Wrong parameter format : %s", line);
                resp_error_param_format(c->worker->msg_buf, c->socket);
                break;

            case DR_OK:
                log_debug("Data OK.");
                break;

            default:
                break;
        }

        enum cmd_func_ret cmd_ret;

        switch(msg_get_stat(c->msg))
        {
            case MSG_STAT_DONE:
                log_debug("Message Done.");
                client_increase_req_ok(c);
                worker_increase_req_ok(c->worker);
                cmd_ret = command_proc(c, c->msg);
                if(cmd_ret != CMD_FUNC_DONE)
                {
                    c->req_ok++;
                    s->req_ok++;
                }
                msg_clean(c->msg);
                break;
        }


//        ic_trans_stat trans_stat = decoder_read_line(c, line);
//        log_debug("Trans stat : %d", trans_stat);
//        if(trans_stat == IC_TRANS_ALL_DONE)
//        {
//            log_debug("Trasfer done, ");
//        }
    }

}

void client_event_write(struct bufferevent *bev, void *arg)
{
    log_debug("client_event_write");
}

