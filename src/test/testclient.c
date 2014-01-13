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

#define TEST_DATA "+PING\n";
// #define TEST_DATA "+GET 1\n$123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"

int done_count = 0;
pthread_mutex_t lock;

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
        sleep(1);
    }

    return sock;
}

int send_data(int sock, const char *key, size_t key_len)
{
    return write(sock, key, key_len);
}

uint64_t check_get_data(const char *ip, int port, const char *key, int count)
{
    int sock, i, len;
    uint64_t start, end;
    size_t key_len;
    char buf[4096];
    memset(buf, 0, 4096);

    sock = init_client_sock(ip, port);
    key_len = strlen(key);

    start = utime_time();

    for(i = 0; i < count; i++)
    {
        len = write(sock, key, key_len);
        len = read(sock, buf, 4096);
    }

    end = utime_time();

    close(sock);
    pthread_mutex_lock(&lock);
    done_count++;
    pthread_mutex_unlock(&lock);

    return end - start;
}

struct run_data
{
    char *ip;
    int port;
    char *key;
    int count;
};

void* thread_func(void *data)
{
    struct run_data *rd = (struct run_data*) data;
    int tid = pthread_self();
//    printf("Starting a thread, #%d\n", tid);
    uint64_t elapsed = check_get_data(rd->ip, rd->port, rd->key, rd->count);
    printf("Stopped thread #%d, %d rounds, elapsed time:%" PRIu64 "s %" PRIu64 "us Avg:%"PRIu64"us\n", tid, rd->count, elapsed/1000000, elapsed%1000000, elapsed/rd->count);
    return NULL;
}

void print_usage(const char *cmd)
{
    printf("Usage: %s IPADDR PORT COUNT\n\n", cmd);
}

int main(int argc, const char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    log_level(_LOG_DEBUG);

    int tcount = 0;
    if(argc < 4)
    {
        print_usage(argv[0]);
        exit(-1);
    }

    if(argc > 4)
    {
        tcount = atoi(argv[4]);
    }


    int tid, i, tcnt;
    struct run_data rdata;
    rdata.ip = (char *)argv[1];
    rdata.port = atoi(argv[2]);
    tcnt = atoi(argv[3]);
    rdata.count = tcnt / tcount;
    char tkey[] = TEST_DATA;
    rdata.key = tkey;



    pthread_mutex_init(&lock, NULL);
    pthread_t threads[tcount];

    for(i = 0; i < tcount; i++)
    {
        tid = pthread_create(&(threads[i]), NULL, thread_func, (void*) &rdata);
        pthread_detach(threads[i]);
    }

    while(1)
    {
        if(done_count >= tcount)
        {
            exit(0);
        }
        usleep(100000);
    }

    return 0;
}
