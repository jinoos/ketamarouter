#ifndef _SETTING_H_
#define _SETTING_H_

#include <stdint.h>
#include <glib.h>

typedef struct setting_t
{
	GKeyFile            *ini;
    GHashTable          *clusters;
} setting_t;

#define SETTING_G_GLOBAL                            "global"
#define SETTING_G_CLUSTER                           "cluster"

#define SETTING_PORT                                "port"
#define SETTING_PORT_DEFAULT                        "7758"

#define SETTING_QPS_QUEUE_SECONDS                   "qps-queue-seconds"
#define SETTING_QPS_QUEUE_SECONDS_DEFAULT           "10"

#define SETTING_NUMBER_OF_WORKERS                   "number-of-workers"
#define SETTING_NUMBER_OF_WORKERS_DEFAULT           "4"

#define SETTING_MAX_CONNECTIONS                     "max-connections"
#define SETTING_MAX_CONNECTIONS_DEFAULT             "10000"

#define SETTING_LOG_LEVEL                           "log-level"
#define SETTING_LOG_LEVEL_DEFAULT                   "warn"      // debug, info, warn, err

setting_t* setting_load(const char *file);
void setting_free(setting_t *set);

int setting_get_port(setting_t *set);
int setting_get_qps_queue_seconds(setting_t *set);
int setting_get_number_of_workers(setting_t *set);
uint64_t setting_get_max_connections(setting_t *set);
int setting_get_log_level(setting_t *set);

#endif // _SETTING_H_
