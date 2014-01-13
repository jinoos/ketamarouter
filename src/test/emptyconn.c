#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include "utime.h"
#include "logging.h"

int init_client_sock(const char *ip, int port)
{
    int i, max_con_retry = 1000;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    for(i = 0; i < max_con_retry; i++)
    {
        if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) >= 0)
        {
            break;
        }
        printf("Cannot connect to server, retry, %s:%d\n", ip, port);
    }

    return sock;
}


void print_usage(const char *cmd)
{
    printf("Usage: %s IPADDR PORT CONNECTIONS SLEEPTIME\n\n", cmd);
}

int main(int argc, const char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    log_level(_LOG_DEBUG);

    int tcount = 1;
    if(argc < 5)
    {
        print_usage(argv[0]);
        exit(-1);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int con_cnt = atoi(argv[3]);
    int sleep_time = atoi(argv[4]);

    int i, conn[con_cnt];

    for(i = 0; i < con_cnt; i++)
    {
        conn[i] = init_client_sock(ip, port);
    }

    sleep(sleep_time);

    for(i = 0; i < con_cnt; i++)
    {
        close(conn[i]);
    }

    return 0;
}
