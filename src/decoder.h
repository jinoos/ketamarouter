#ifndef _DECODER_H_
#define _DECODER_H_

#include "message.h"

typedef enum decode_result
{
    DR_OK                   = 0,
    DR_ERR_FORMAT           = 1,
    DR_ERR_PARAM_FORMAT     = 2

} decode_result;

decode_result decoder_command(msg_t *msg, char *row);
decode_result decoder_param(msg_t *msg, char *row);
decode_result decoder_run(msg_t *msg, char *row);

#endif // _DECODER_H_
