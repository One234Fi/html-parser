//https://nullprogram.com/blog/2023/09/27/

#ifndef ARENA_MODULE_H
#define ARENA_MODULE_H

#include "types.h"
#include <stddef.h>

#define new(...)                    newx(__VA_ARGS__,new4,new3,new2)(__VA_ARGS__)
#define newx(a, b, c, d, e, ...)    e
#define new2(a, t)                  (t *)alloc(a, sizeof(t), _Alignof(t), 1)
#define new3(a, t, n)               (t *)alloc(a, sizeof(t), _Alignof(t), n)

typedef struct arena arena;
struct arena {
    char * beg;
    char * pos;
    char * end;
};

arena arena_init(ptrdiff_t cap);
arena arena_wrap(ptrdiff_t cap, void * mem);
void * alloc(arena * a, size stride, size align, size count);


#endif
