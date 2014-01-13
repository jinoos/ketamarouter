#include <string.h>

#include "decoder.h"

decode_result decoder_command(msg_t *msg, char *row)
{
    size_t clen, len = strlen(row);
    char *c, *s;
    cmd_t *cmd;

    if(len <= 0 || row[0] != MSG_CMD_HEADER)
        return DR_ERR_FORMAT;

    c = row + 1;

    if(strlen(c) <= 0)
        return DR_ERR_FORMAT;

    s = strchr(c, ' ');

    if(s == NULL)
    {
        cmd = msg_init_cmd(msg->buf, c, strlen(c), 0, row);
        msg_set_cmd(msg, cmd);
        return DR_OK;
    }

    clen = s - c;
    s++;

    if(strlen(s) <= 0)
        return DR_ERR_FORMAT;

    uint32_t param_count = (uint32_t)atoi(s);

    if(param_count <= 0)
        return DR_ERR_FORMAT;

    cmd = msg_init_cmd(msg->buf, c, clen, param_count, row);
    msg_set_cmd(msg, cmd);
    return DR_OK;
}

decode_result decoder_param(msg_t *msg, char *row)
{
    char *s, *k, *v;
    size_t klen;
    param_t *param;

    switch(row[0])
    {
        case MSG_STR_HEADER:
            s = row + 1;
            param = msg_init_param_str(msg->buf, s, strlen(s), row);
            msg_add_param(msg, param);
            return DR_OK;
            break;

        case MSG_KV_HEADER:
            k = row + 1;
            v = strchr(k, MSG_KV_SEPT);
            if(v == NULL)
                return DR_ERR_PARAM_FORMAT;

            klen = v - k;
            v++;
            param = msg_init_param_kv(msg->buf, k, klen, v, strlen(v), row);
            msg_add_param(msg, param);
            return DR_OK;
            break;

        default:
            return DR_ERR_PARAM_FORMAT;
    }

    return DR_OK;
}

decode_result decoder_run(msg_t *msg, char *row)
{
    switch(msg_get_stat(msg))
    {
        case MSG_STAT_NONE:
            return decoder_command(msg, row);
            break;

        case MSG_STAT_DATA_MORE:
            return decoder_param(msg, row);
            break;

        default:
            break;
    }
    return DR_OK;
}

