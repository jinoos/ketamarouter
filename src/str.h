/*
 * common.h
 *
 *  Created on: 2013. 12. 9.
 *      Author: Jinoos Lee<jinoos@gmail.com>
 */

#ifndef STR_H_
#define STR_H_

#include <glib.h>
#include "pabuf.h"

enum str_type
{
    STR_TYPE_NONE = 0,
    STR_TYPE_ALLOC = 1
};

typedef struct str_t
{
    enum str_type   type;
    char            *data;
    size_t          len;
} str_t;

// @brief Init str_t by pabuf
// @param pabuf pointer 
// @param string pointer
// @param size of string
str_t* bstr_init(pabuf_t *pabuf, char *data, size_t len);

// @brief Init str_t by pabuf with allocation new string 
// @param pabuf pointer 
// @param string pointer
// @param size of string
str_t* bstr_init_alloc(pabuf_t  *pabuf, char *data, size_t len);

// @brief Return back a str_t to pabuf
// @param pabuf pointer
// @param str_t object
void bstr_free(pabuf_t *pabuf, str_t *str);

str_t* str_init(char *data, size_t len);
str_t* str_init_alloc(char *data, size_t len);
void str_free(str_t *str);

gboolean str_equal(gconstpointer a, gconstpointer b);
guint str_hash(gconstpointer v);

#endif /* STR_H_ */
