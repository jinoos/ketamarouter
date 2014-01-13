#include <stdlib.h>

#include "pabuf.h"
#include "logging.h"

void pabuf_print(pabuf_t *p)
{
    log_debug("PABUF - OBJSIZE:%zu MEM:%zu SMEM:%zu CNT:%"PRIu32" MAXRCNT:%"PRIu32" MAXSCNT:%"PRIu32" RCNT:%"PRIu32" ESCNT:%"PRIu32"",
                        p->obj_size,
                        p->memory,
                        p->shell_memory,
                        p->count,
                        p->max_remain_count,
                        p->max_eshell_count,
                        p->remain_count,
                        p->eshell_count);
}

pabuf_shell_t* pabuf_get_eshell(pabuf_t *pabuf, void *obj)
{
    pabuf_shell_t *shell = NULL;
    if(pabuf->eshell_count == 0)
    {
        pabuf->shell_memory += sizeof(pabuf_shell_t);
        shell = (pabuf_shell_t*) calloc(sizeof(pabuf_shell_t), 1);
        shell->obj = obj;

        return shell;
    }
    
    shell = pabuf->first_eshell;
    pabuf->first_eshell = shell->next;
    shell->next = NULL;
    pabuf->eshell_count--;
    shell->obj = obj;
    return shell;
}

void pabuf_back_eshell(pabuf_t *pabuf, pabuf_shell_t *shell)
{
    if(pabuf->eshell_count >= pabuf->max_eshell_count)
    {
        pabuf->shell_memory -= sizeof(pabuf_shell_t);
        free(shell);
        return;
    }

    shell->next = pabuf->first_eshell;
    pabuf->first_eshell = shell;
    shell->obj = NULL;
    pabuf->eshell_count++;
    return;
}

void* pabuf_get(pabuf_t *pabuf)
{
#ifndef PABUF_NOLOCK
    pthread_mutex_lock(&pabuf->lock);
#endif
    if(pabuf->remain_count <= 0)
    {
        pabuf->memory += pabuf->obj_size;
        pabuf->count++;
        //pabuf_print(pabuf);
#ifndef PABUF_NOLOCK
        pthread_mutex_unlock(&pabuf->lock);
#endif
        return calloc(pabuf->obj_size, 1);
    }

    pabuf_shell_t *shell = pabuf->first;
    pabuf->first = shell->next;
    void *obj = shell->obj;
    pabuf_back_eshell(pabuf, shell);
    pabuf->remain_count--;
    //pabuf_print(pabuf);
#ifndef PABUF_NOLOCK
    pthread_mutex_unlock(&pabuf->lock);
#endif
    return obj;
}

void pabuf_back(pabuf_t *pabuf, void *obj)
{
#ifndef PABUF_NOLOCK
    pthread_mutex_lock(&pabuf->lock);
#endif
    if(pabuf->max_remain_count <= pabuf->remain_count)
    {
        pabuf->memory -= pabuf->obj_size;
        pabuf->count--;
        //pabuf_print(pabuf);
#ifndef PABUF_NOLOCK
        pthread_mutex_unlock(&pabuf->lock);
#endif
        return free(obj);
    }

    pabuf_shell_t *shell = pabuf_get_eshell(pabuf, obj);
    shell->next = pabuf->first;
    pabuf->first = shell;
    pabuf->remain_count++;
    //pabuf_print(pabuf);
#ifndef PABUF_NOLOCK
    pthread_mutex_unlock(&pabuf->lock);
#endif
    return;
}

void pabuf_return_back(pabuf_t *pabuf, void *obj)
{
    pabuf_back(pabuf, obj);
}

pabuf_t* _pabuf_init(size_t obj_size, uint32_t max_remain_count, uint32_t max_eshell_count)
{
    pabuf_t *pabuf = calloc(sizeof(pabuf_t), 1);
    pabuf->max_remain_count = max_remain_count;
    pabuf->max_eshell_count = max_eshell_count;
    pabuf->obj_size = obj_size;
#ifndef PABUF_NOLOCK
    pthread_mutex_init(&pabuf->lock, NULL);
#endif

    return pabuf;
}

pabuf_t* pabuf_init(size_t obj_size)
{
    return _pabuf_init(obj_size, PABUF_DEFAULT_MAX_REMAIN_COUNT, PABUF_DEFAULT_MAX_ESHELL_COUNT);
}

pabuf_t* pabuf_init2(size_t obj_size, uint32_t max_remain_count)
{
    return _pabuf_init(obj_size, max_remain_count, PABUF_DEFAULT_MAX_ESHELL_COUNT);
}

pabuf_t* pabuf_init3(size_t obj_size, uint32_t max_remain_count, uint32_t max_eshell_count)
{
    return _pabuf_init(obj_size, max_remain_count, max_eshell_count);
}
