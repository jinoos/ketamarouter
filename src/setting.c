#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "setting.h"
#include "logging.h"

enum setting_opt_mandatory
{
    OPT_MANDATORY,
    OPT_OPTIONAL
};

struct setting_mandatory_group_t
{
    const char *name;
};

struct setting_opt_t
{
    const char *group;
    const char *key;
    enum setting_opt_mandatory type;
    const char *defval;
};

struct setting_mandatory_group_t groups[] =
{
    {SETTING_G_GLOBAL},
    {SETTING_G_CLUSTER},
    {NULL}
};

struct setting_opt_t options[] = 
{
    {SETTING_G_GLOBAL, SETTING_PORT, OPT_MANDATORY, ""},
    {SETTING_G_GLOBAL, SETTING_QPS_QUEUE_SECONDS, OPT_OPTIONAL, SETTING_QPS_QUEUE_SECONDS_DEFAULT},
    {SETTING_G_GLOBAL, SETTING_NUMBER_OF_WORKERS, OPT_OPTIONAL, SETTING_NUMBER_OF_WORKERS_DEFAULT},
    {SETTING_G_GLOBAL, SETTING_MAX_CONNECTIONS, OPT_OPTIONAL, SETTING_MAX_CONNECTIONS_DEFAULT},
    {SETTING_G_GLOBAL, SETTING_LOG_LEVEL, OPT_OPTIONAL, SETTING_LOG_LEVEL_DEFAULT},
    {NULL, NULL, 0, NULL}
};

int setting_check_mandatory(GKeyFile *ini, const char *file)
{
    int i;
    GError *err;

    for(i=0; groups[i].name != NULL; i++)
    {
        struct setting_mandatory_group_t *g = &groups[i];
        
        if(!g_key_file_has_group(ini, g->name))
        {
            log_err("Option group '%s' must be existed in config file : %s", g->name, file);
            exit(-1);
        }
    }

    for(i=0; options[i].group != NULL; i++)
    {
        err = NULL;
        struct setting_opt_t *opt = &options[i];

        if(opt->group == NULL && opt->key == NULL)
        {
            break;
        }

        switch(opt->type)
        {
            case OPT_MANDATORY:
                if(!g_key_file_has_key(ini, opt->group, opt->key, &err))
                {
                    log_err("Option '%s:%s' must be existed in config file : %s", opt->group, opt->key, file);
                    log_err("Option error, %s", err->message);
                    exit(-1);
                }
                break;
            case OPT_OPTIONAL:

                if(!g_key_file_has_key(ini, opt->group, opt->key, &err))
                {
                    log_info("Option '%s:%s' set default '%s'", opt->group, opt->key, opt->defval);
                    g_key_file_set_value(ini, opt->group, opt->key, opt->defval);
                }
                break;

            default:
                break;
        }
    }
}

setting_t* setting_load(const char *file)
{
	log_info("Loading config file, %s", file);

	if( access( file, R_OK ) == -1 )
	{
		log_err("Cannot read config file, %s", file);
		return NULL;
	}

	GKeyFile	*ini;
	GKeyFileFlags ini_flags;
	GError *error = NULL;

	ini = g_key_file_new ();
	ini_flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	setting_t *setting = calloc(sizeof(setting_t), 1);
	assert(setting != NULL);

    setting->clusters = g_hash_table_new(g_str_hash, g_str_equal);

	if(!g_key_file_load_from_file(ini, file, ini_flags, &error))
	{
		log_err("Config loading error, %s. %s", error->message, file);
		setting_free(setting);
		return NULL;
	}

    setting_check_mandatory(ini, file);

	setting->ini = ini;
	return setting;
}


void setting_free(setting_t *set)
{
	if(set == NULL)
		return;
	if(set->ini != NULL)
		g_key_file_free(set->ini);

	free(set);
}

int setting_get_port(setting_t *set)
{
    GError *err = NULL;
    int v = g_key_file_get_integer(set->ini, SETTING_G_GLOBAL, SETTING_PORT, &err);
    return v;
}

int setting_get_qps_queue_seconds(setting_t *set)
{
    GError *err = NULL;
    int v = g_key_file_get_integer(set->ini, SETTING_G_GLOBAL, SETTING_QPS_QUEUE_SECONDS, &err);
    return v;
}

int setting_get_number_of_workers(setting_t *set)
{
    GError *err = NULL;
    int v = g_key_file_get_integer(set->ini, SETTING_G_GLOBAL, SETTING_NUMBER_OF_WORKERS, &err);
    return v;
}

uint64_t setting_get_max_connections(setting_t *set)
{
    GError *err = NULL;
    uint64_t v = (uint64_t) g_key_file_get_int64(set->ini, SETTING_G_GLOBAL, SETTING_MAX_CONNECTIONS, &err);
    
    return v;
}

int setting_get_log_level(setting_t *set)
{
    GError *err = NULL;
    char *lstr = (char*)g_key_file_get_string(set->ini, SETTING_G_GLOBAL, SETTING_LOG_LEVEL, &err);

    if(strncasecmp(lstr, _LOG_SDEBUG, 5) == 0)
    {
        log_msg("Log level is DEBUG");
        return _LOG_DEBUG;
    }else if(strncasecmp(lstr, _LOG_SINFO, 4) == 0)
    {
        log_msg("Log level is INFO");
        return _LOG_INFO;
    }else if(strncasecmp(lstr, _LOG_SWARN, 4) == 0)
    {
        log_msg("Log level is WARN");
        return _LOG_WARN;
    }else if(strncasecmp(lstr, _LOG_SERR, 5) == 0)
    {
        log_msg("Log level is ERROR");
        return _LOG_ERR;
    }else
    {
        log_msg("Log level is WARN");
        return _LOG_DEFAULT;
    }
}


