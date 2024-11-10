#ifndef COMMON_MEM_MODULE
#define COMMON_MEM_MODULE

#include <stdio.h>

#define ALLOCATE(ptr, newsize) ptr = realloc(ptr, newsize)
#define FREE(ptr) if (ptr) { free(ptr); ptr = NULL; }

void * MALLOC(const size_t size);
void * REALLOC(void * p, const size_t size);

#endif
