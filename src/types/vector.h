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

#include "types/arena.h"
#include "types/types.h"
#include <assert.h>
#include <stddef.h>

void grow(void * slice, size stride, size align, arena * a);

#define push(s, arena)  ((s)->len >= (s)->cap ? grow(s, sizeof(*(s)->data), _Alignof(*(s)->data), arena), (s)->data + (s)->len++ : (s)->data + (s)->len++) 

#endif
