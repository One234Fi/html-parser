//https://nullprogram.com/blog/2023/09/27/

#ifndef ARENA_MODULE_H
#define ARENA_MODULE_H

#include <stddef.h>

#define new(...)                    newx(__VA_ARGS__,new4,new3,new2)(__VA_ARGS__)
#define newx(a, b, c, d, e, ...)    e
#define new2(a, t)                  (t *)alloc(a, sizeof(t), _Alignof(t), 1)
#define new3(a, t, n)               (t *)alloc(a, sizeof(t), _Alignof(t), n)

struct arena {
    char * beg;
    char * end;
};
typedef struct arena arena;

arena arena_init(ptrdiff_t cap);
void * alloc(arena * a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);

#endif
