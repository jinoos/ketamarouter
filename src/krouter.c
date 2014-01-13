#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#include <glib.h>

#include "version.h"
#include "krouter.h"
#include "setting.h"
#include "logging.h"
#include "logging.h"
#include "server.h"
#include "worker.h"
#include "event_util.h"

void* server_thread_func(void *data)
{
    log_info("Start server thread.");
    server_t *server = (server_t*) data;
	server_start(server);
}

void* worker_thread_func(void *data)
{
    log_info("Start worker thread.");
    worker_t *w = (worker_t*) data;
    worker_start(w);
}

void* sentinel_thread_func(void *data)
{
    log_info("Start sentinel thread.");
    server_t *server = (server_t*) data;

}

void signal_handler(int sig)
{
    switch(sig)
    {
        case SIGHUP:
        case SIGINT:
            log_info("Signal received %s", strsignal(sig));
            break;

        default:
            log_debug("Signal received %s", strsignal(sig));
            break;
    }
}

void set_signal_handler()
{
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    //signal(SIGPIPE, SIG_IGN);
    signal(SIGPIPE, signal_handler);

    return;
}

void print_version(const char *command)
{
    printf("\nKrouter. version %d.%d.%d\n", _VER_MAJOR, _VER_MINOR, _VER_PATCH);
    return;
}

void print_help(const char *command)
{
    print_version(command);
    printf("Usage: %s [CONFIG_FILE | -v | -h ]\n", command);
    printf("    -v        Print version.\n");
    printf("    -h        This help.\n");
    printf("Example:\n");
    printf("    %s ./file.conf \n", command);
    printf("    %s -v\n", command);
    printf("    %s -h\n", command);
    exit(0);
    return;
}

int main(int argc, char **argv)
{
    set_signal_handler();
	log_level(_LOG_DEBUG);
	//log_level(_LOG_INFO);
	//log_level(_LOG_ERR);

    event_set_log_callback(event_log_callback);
    event_enable_debug_mode();
//    event_use_pthreads();

    char *conf_file;
    int opt;

    if(argc < 2)
    {
        print_help(argv[0]);
        exit(-1);
    }

    int wt = 0;

    while((opt = getopt(argc, argv, "hvt:")) != -1)
    {
        switch(opt)
        {
            case 'h':
                print_help(argv[0]);
                exit(0);
                break;
            case 'v':
                print_version(argv[0]);
                exit(0);
                break;
            case 't':
                wt = atoi(optarg);
                break;
        }
    }

    conf_file = argv[1];

    pthread_t server_thread, sentinel_thread;
    int server_thread_id, sentinel_thread_id;

	setting_t *setting = setting_load(conf_file);

	if(setting == NULL)
	{
		log_err("Cannot load config file.");
		return EXIT_FAILURE;
	}

	log_level(setting_get_log_level(setting));

    int port = setting_get_port(setting);

    server_t *server;
    if(wt != 0)
    {
        server = server_init(setting_get_port(setting), wt);
    }else
    {
        server = server_init(setting_get_port(setting), setting_get_number_of_workers(setting));
    }

    server->setting = setting;

    int i, worker_id;
    for(i = 0; i < server->worker_count; i++)
    {
        worker_id = pthread_create(server->worker_thread[i], NULL, worker_thread_func, (void*)server->worker_list[i]);
        assert(worker_id >= 0);
        pthread_detach(*(server->worker_thread[i]));
    }

    sleep(1);

    server_thread_id = pthread_create(&server_thread, NULL, server_thread_func, (void*) server);
    assert(server_thread_id >= 0);
    pthread_detach(server_thread);

    pause();

	return EXIT_SUCCESS;
}
