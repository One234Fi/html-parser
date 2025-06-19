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
#include "types.h"
#include <assert.h>
#include <stddef.h>

void grow(void * slice, size stride, arena * a);
void shift_right_impl(void * slice, size index, size stride, arena * a);
void shift_left_impl(void * slice, size index, size stride);


#define push(s, arena)  ((s)->len >= (s)->cap ? grow(s, sizeof(*(s)->data), arena), (s)->data + (s)->len++ : (s)->data + (s)->len++) 
#define push_front(s, arena) (shift_right_impl(s, 1, sizeof(*(s)->data), arena), (s)->data)


#endif
