#ifndef VECTOR_MODULE
#define VECTOR_MODULE

/* vector definition
 *
 *  struct {
 *      void * data;
 *      ptrdiff_t len;
 *      ptrdiff_t cap;
 *  };
 */

#include "arena.h"
#include <assert.h>
#include <stddef.h>

void grow(void * slice, ptrdiff_t size, ptrdiff_t align, arena * a);

#define push(s, arena) (\
    (s)->len < (s)->cap ? \
        (s)->data + (s)->len++ : \
        grow(s, sizeof(*(s)->data), _Alignof(*(s)->data), arena), (s)->data + (s)->len++ \
) 


//TODO delete/replace this
typedef struct vector vector;

vector * vector_init(size_t item_size);
void vector_destroy(vector * v);
void vector_push_back(vector * v, void * item);
const void * vector_pop_back(vector * v);
const void * vector_get(const vector * v, const size_t index);
//TODO delete/replace this

#endif
