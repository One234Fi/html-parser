#ifndef MEM_MODULE_H
#define MEM_MODULE_H

#include "types/types.h"

void * xmalloc(size s);
void * xrealloc(void * ptr, size s);

#define xfree(ptr) free(ptr); ptr = NULL

#endif
