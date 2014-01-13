#ifndef _PABUF_H_
#define _PABUF_H_

#include <stddef.h>
#include <stdint.h>

#include "logging.h"

//
// PABuffer - Pre-Allocated Buffer
//
#define PABUF_DEFAULT_MAX_REMAIN_COUNT       100*1
#define PABUF_DEFAULT_EXT_COUNT              10

typedef struct pabuf_shell
{
    void                *obj;
    struct pabuf_shell  *next;
} pabuf_shell;

typedef struct pabuf pabuf;
struct pabuf
{
    // size of allocated objects
    uint32_t            capacity;

    // consumed memory size;
    size_t				memory_size;

    // size of a object
    size_t              obj_size;

    // max number of remain objects
    uint32_t            max_remain_count;

    // number of remain buffer objects
    uint32_t            remain_count;

    // number of object to extend when buffer is empty
    uint32_t            exp_count;

    // head of available object shell
    pabuf_shell      *first_shell;

    // head of empty shell
    pabuf_shell      *first_empty_shell;
};

pabuf*   pabuf_init(size_t obj_size);
pabuf*   pabuf_init2(size_t obj_size, uint32_t max_remain_count);
pabuf*   pabuf_init3(size_t obj_size, uint32_t max_remain_count, uint32_t exp_count);

uint32_t    pabuf_capacity(pabuf *pabuf);
size_t		pabuf_mem_size(pabuf *pabuf);

void*       pabuf_get(pabuf *pabuf);
void        pabuf_return_back(pabuf *pabuf, void *obj);
void        pabuf_back(pabuf *pabuf, void *obj);
int			pabuf_free(pabuf *pabuf);

#endif // _PABUF_H_
