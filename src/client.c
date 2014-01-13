#include <string.h>
#include <time.h>

#include "client.h"

client_t* client_init(worker_t *w, evutil_socket_t c_sock, struct sockaddr_in *c_addr)
{
    client_t *client = calloc(sizeof(client_t), 1);
    client->worker = w;
    client->server = w->server;
    client->socket = c_sock;
    memcpy(&(client->addr), c_addr, sizeof(struct sockaddr_in));
//    client->msg = msg_init(w->msg_buf);
    client->last_time = time(NULL);

    return client;
}

void client_free(client_t *client)
{
    if(client->evbuf != NULL)
        evbuffer_free(client->evbuf);

    msg_free(client->msg);

    return;
}

void client_increase_req_all(client_t *c)
{
    c->req_all++;
}

void client_increase_req_ok(client_t *c)
{
    c->req_all++;
    c->req_ok++;
}

void client_increase_req_err(client_t *c)
{
    c->req_all++;
    c->req_err++;
}

