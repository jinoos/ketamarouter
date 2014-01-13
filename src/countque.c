#include <stdlib.h>
#include <string.h>
#include "countque.h"

inline time_t get_base_time_by_time(time_t term, time_t t)
{
	return t-(t%term);
}

inline time_t get_base_time(time_t term)
{
    time_t t = time(NULL);
    return get_base_time_by_time(term, t);
}

inline time_t get_ttl_time(time_t base_time, int tic_per_sec, int tic_max_cnt)
{
	return (time_t)base_time - (tic_per_sec * tic_max_cnt);
}

inline countque_tic_t* cq_tic_init(countque_t *cqi)
{
	countque_tic_t *tic = (countque_tic_t*) pabuf_get(cqi->tic_buffer);
	tic->count = 0;
	tic->newer = NULL;
	return tic;
}

inline void cq_tic_free(countque_t *cqi, countque_tic_t *tic)
{
    pabuf_back(cqi->tic_buffer, tic);
}

inline void cq_adjust(countque_t *cqi, time_t base_time)
{
	time_t ttl = get_ttl_time(base_time, cqi->tic_per_sec, cqi->tic_max_cnt);
	countque_tic_t *tic = cqi->first;
	while(tic != NULL && tic->time <= ttl)
	{
		cqi->first = tic->newer;
		cqi->count -= tic->count;
		pabuf_back(cqi->tic_buffer, tic);
		tic = cqi->first;
	}

    if(tic == NULL)
    {
        cqi->first = cqi->last = NULL;
    }
}

inline uint64_t cq_count(countque_t *cqi)
{
    time_t base_time = get_base_time(cqi->tic_per_sec);
    cq_adjust(cqi, base_time);
    return cqi->count;
}

inline countque_t* cq_init(time_t tic_per_sec, uint32_t tic_max_cnt, pabuf_t *item_buffer, pabuf_t *tic_buffer)
{
	countque_t *item = (countque_t*) pabuf_get(item_buffer);
    item->count = 0;
    item->tic_per_sec = tic_per_sec;
    item->tic_max_cnt = tic_max_cnt;
    item->first = item->last = NULL;
    item->tic_buffer = tic_buffer;
    item->item_buffer = item_buffer;

    return item;
}

inline uint64_t cq_tic(countque_t *cqi)
{
    time_t t = time(NULL);
    return cq_tic_time(cqi, t);
}

inline uint64_t cq_tic_time(countque_t *cqi, time_t t)
{
    time_t base_time = get_base_time_by_time(cqi->tic_per_sec, t);

	countque_tic_t *tic;
	if(cqi->last == NULL)
	{
		tic = cq_tic_init(cqi);
		tic->time = base_time;
		tic->count++;

		cqi->first = cqi->last = tic;

	}else
	{
		if(cqi->last->time == base_time)
		{
			cqi->last->count++;
		}else
		{
			tic = cq_tic_init(cqi);
			tic->time = base_time;
			tic->count++;
			cqi->last->newer = tic;
			cqi->last = tic;
		}
    }

	cqi->count++;

    cq_adjust(cqi, base_time);

	return cqi->count;
}

inline void cq_free(countque_t *cqi)
{
    pabuf_t *item_buffer = cqi->item_buffer;
    countque_tic_t *tic = cqi->first;
    while(tic != NULL)
    {
        cqi->first = tic->newer;
        pabuf_back(cqi->tic_buffer, tic);
        tic = cqi->first;
    }

    pabuf_back(item_buffer, cqi);
    return;
}

