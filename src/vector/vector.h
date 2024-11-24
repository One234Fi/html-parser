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

#include "arena/arena.h"
#include <assert.h>
#include <stddef.h>

void grow(void * slice, ptrdiff_t size, ptrdiff_t align, arena * a);

#define push(s, arena)  ((s)->len >= (s)->cap ? grow(s, sizeof(*(s)->data), _Alignof(*(s)->data), arena), (s)->data + (s)->len++ : (s)->data + (s)->len++) 

#endif
