#include <stdlib.h>

#include "pabuf.h"

int pabuf_extend(pabuf* pabuf, size_t obj_size, uint32_t size);
pabuf_shell* pabuf_alloc(size_t obj_size);

pabuf* pabuf_init(size_t obj_size)
{
	return pabuf_init3(obj_size, PABUF_DEFAULT_MAX_REMAIN_COUNT, PABUF_DEFAULT_EXT_COUNT);
}

pabuf* pabuf_init2(size_t obj_size, uint32_t max_remain_count)
{
	return pabuf_init3(obj_size, max_remain_count, PABUF_DEFAULT_EXT_COUNT);
}


pabuf* pabuf_init3(size_t obj_size, uint32_t max_remain_count, uint32_t exp_count)
{
	int ret;

    pabuf *pabuf = calloc(sizeof(pabuf), 1);
    if(pabuf == NULL)
        return NULL;

    pabuf->obj_size = obj_size;
    pabuf->memory_size += sizeof(pabuf);
    pabuf->max_remain_count = max_remain_count;
    pabuf->exp_count = exp_count;

    ret = pabuf_extend(pabuf, obj_size, exp_count);

    if(ret == 0)
    {
    	free(pabuf);
    	return NULL;
    }

    return pabuf;
}

int pabuf_extend(pabuf* pabuf, size_t obj_size, uint32_t size)
{
    int i;
    pabuf_shell *shell = NULL;
    for(i = 0; i < size; i++)
    {
        shell = pabuf_alloc(pabuf->obj_size);

        if(shell == NULL)
            break;

        shell->next = pabuf->first_shell;
        pabuf->first_shell = shell;
        pabuf->capacity++;
        pabuf->remain_count++;
        pabuf->memory_size += (obj_size + sizeof(pabuf_shell));
    }

    return i;
}

pabuf_shell* pabuf_alloc(size_t obj_size)
{
    pabuf_shell *shell = (pabuf_shell*)calloc(sizeof(pabuf_shell), 1);
    if(shell == NULL)
        return NULL;

    log_err("calloc for obj %d", obj_size);
    shell->obj = calloc(obj_size, 1);
    if(shell->obj == NULL)
    {
        free(shell);
        return NULL;
    }

    return shell;
}

uint32_t pabuf_capacity(pabuf *pabuf)
{
	return pabuf->capacity;
}

// @brief Get size of memory 
uint64_t pabuf_mem_size(pabuf *pabuf)
{
	return pabuf->memory_size;
}

// @brief Obtain a object to use, you must cast to proper object
// @param Pabuffer pointer
// @return object, it must be casted to proper object
void* pabuf_get(pabuf *pabuf)
{
	if(pabuf->first_shell == NULL)
	{
		if(pabuf_extend(pabuf, pabuf->obj_size, pabuf->exp_count) == 0)
		{
			return NULL;
		}
	}

	pabuf_shell *shell = pabuf->first_shell;
	pabuf->first_shell = shell->next;

    void *obj = shell->obj;
    shell->obj = NULL;

	shell->next = pabuf->first_empty_shell;
	pabuf->first_empty_shell = shell;
	pabuf->remain_count--;
	return obj;
}

// @bfief Return back object into pabuf
// @param Pabuffer pointer
// @param Object which is finished to use
void pabuf_return_back(pabuf *pabuf, void *obj)
{
	pabuf_shell *shell = pabuf->first_empty_shell;
	pabuf->first_empty_shell = shell->next;

	if(pabuf->max_remain_count <= pabuf->remain_count)
	{
		free(shell);
		free(obj);
		pabuf->memory_size -= (sizeof(pabuf_shell)+pabuf->obj_size);
		pabuf->capacity--;
	}else
	{
		shell->obj = obj;
		shell->next = pabuf->first_shell;
		pabuf->first_shell = shell;
		pabuf->remain_count++;
	}
	return;
}

void pabuf_back(pabuf *pabuf, void *obj)
{
    pabuf_return_back(pabuf, obj);
}

// CAUTION Don't call pabuf_free before return all buffer objects
//          back into pabuf. If not, will be passible to occurred
//          memory leak or sagmant-fault.
// 
// @brief Release all allocated memory (if passible)
// @param Pabuffer pointer
// @returns
//		0 = if pabuf is null
//		0 < number of released objects
int pabuf_free(pabuf *pabuf)
{
	if(pabuf == NULL)
		return 0;

	void *obj;
	pabuf_shell *shell;
	uint32_t count;

	while(pabuf->first_shell != NULL)
	{
		shell = pabuf->first_shell;
		pabuf->first_shell = shell->next;
		obj = shell->obj;
		free(obj);
		free(shell);
		count++;
	}

	while(pabuf->first_empty_shell != NULL)
	{
		shell = pabuf->first_empty_shell;
		pabuf->first_empty_shell = shell->next;
		free(shell);
	}

	free(pabuf);

	return count;
}
