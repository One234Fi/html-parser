#ifndef VECTOR_MODULE
#define VECTOR_MODULE

#include <stdio.h>
typedef struct vector vector;

vector * vector_init(size_t item_size);
void vector_destroy(vector * v);
void vector_push_back(vector * v, void * item);
const void * vector_pop_back(vector * v);
const void * vector_get(const vector * v, const size_t index);

#endif
