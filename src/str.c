#include <stdlib.h>
#include <string.h>
#include "pabuf.h"
#include "str.h"

str_t* _bstr_init(pabuf_t *pabuf, enum str_type type, char *data, size_t len)
{
    str_t *str = (str_t*) pabuf_get(pabuf);
    str->data = data;
    str->len = len;
    str->type = type;
    return str;
}
// @brief Init str_t by pabuf
// @param pabuf pointer 
// @param string pointer
// @param size of string
str_t* bstr_init(pabuf_t *pabuf, char *data, size_t len)
{
    return _bstr_init(pabuf, STR_TYPE_NONE, (char*) data, len);
}

// @brief Init str_t by pabuf with allocation new string 
// @param pabuf pointer 
// @param string pointer
// @param size of string
str_t* bstr_init_alloc(pabuf_t  *pabuf, char *data, size_t len)
{
    char *ndata = malloc(len + 1);
    ndata[len] = 0;
    memcpy(ndata, data, len);
    return _bstr_init(pabuf, STR_TYPE_ALLOC, (char*) ndata, len);
}

// @brief Return back a str_t to pabuf
// @param pabuf pointer
// @param str_t object
void bstr_free(pabuf_t *pabuf, str_t *str)
{
    if(str->type == STR_TYPE_ALLOC)
        free(str->data);
    pabuf_return_back(pabuf, (void*)str);
}

str_t* _str_init(enum str_type type, char *data, size_t len)
{
    str_t *str = (str_t*) malloc(sizeof(str_t));
    str->data = data;
    str->len = len;
    str->type = type;
    return str;
}

str_t* str_init(char *data, size_t len)
{
    return _str_init(STR_TYPE_NONE, data, len);
}

str_t* str_init_alloc(char *data, size_t len)
{
    char *ndata = malloc(len + 1);
    memcpy(ndata, data, len);
    return _str_init(STR_TYPE_ALLOC, ndata, len);
}

void str_free(str_t *str)
{
    if(str->type == STR_TYPE_ALLOC)
        free(str->data);
    free(str);
}

// for glib
gboolean str_equal(gconstpointer a, gconstpointer b)
{
    str_t *astr = (str_t*) a;
    str_t *bstr = (str_t*) b;

    if(astr->len != bstr->len)
        return FALSE;

    if(strncmp(astr->data, bstr->data, astr->len) != 0)
    {
        return FALSE;
    }else
    {
        return TRUE;
    }
}


guint str_hash(gconstpointer v)
{
    str_t *str = (str_t*) v;

    const signed char *p;
    int i;
    guint32 h = 5381;

    for (p = (const signed char*)str->data, i=0; i < str->len; p++, i++)
    {
        h = (h << 5) + h + *p;
    }

    return h;
}
