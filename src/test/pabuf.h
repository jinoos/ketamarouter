#ifndef _PABUF_H_
#define _PABUF_H_

#include <stdint.h>

typedef struct pabuf_shell_t
{
    void                    *obj;
    struct pabuf_shell_t    *next;
} pabuf_shell_t;

typedef struct pabuf_t
{
    size_t              obj_size;
    size_t              memory;

    uint32_t            count;

    uint32_t            max_remain_count;
    uint32_t            max_eshell_count;

    uint32_t            remain_count;
    uint32_t            eshell_count;

    pabuf_shell_t       *first;

    pabuf_shell_t       *first_eshell;
} pabuf_t;

#define PABUF_DEFAULT_MAX_REMAIN_COUNT       1000*1
#define PABUF_DEFAULT_MAX_ESHELL_COUNT       1000*1

void* pabuf_get(pabuf_t *pabuf);
void pabuf_back(pabuf_t *pabuf, void *obj);
void pabuf_return_back(pabuf_t *pabuf, void *obj);
pabuf_t* pabuf_init(size_t obj_size);
pabuf_t* pabuf_init2(size_t obj_size, uint32_t max_remain_count);
pabuf_t* pabuf_init3(size_t obj_size, uint32_t max_remain_count, uint32_t max_eshell_count);

#endif // _PABUF_H_
