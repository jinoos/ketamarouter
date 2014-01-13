#ifndef _COUNTERQUE_H_
#define _COUNTERQUE_H_

#include <stdint.h>
#include <time.h>

#include "pabuf.h"

typedef struct countque_tic_t
{
	uint32_t			count;
	time_t				time;
	struct countque_tic_t		*newer;
} countque_tic_t;

typedef struct countque_t
{
	uint64_t			count;

    time_t              tic_per_sec;
    uint32_t            tic_max_cnt;

	countque_tic_t			*first;
	countque_tic_t			*last;

    pabuf_t             *tic_buffer;
    pabuf_t             *item_buffer;
} countque_t;

inline countque_t* cq_init(time_t tic_per_sec, uint32_t tic_max_cnt, pabuf_t *item_buffer, pabuf_t *tic_buffer);
inline uint64_t cq_tic(countque_t *cqi);
inline uint64_t cq_tic_time(countque_t *cqi, time_t time);
inline uint64_t cq_count(countque_t *cqi);
inline void cq_free(countque_t *cqi);

#endif // _COUNTERQUE_H_
