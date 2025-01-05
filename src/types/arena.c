#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types/arena.h"


arena
arena_init(ptrdiff_t cap) {
    arena a = {0};
    a.beg = malloc(cap);
    a.end = a.beg ? a.beg + cap : 0;
    return a;
}

void * alloc(arena * a, size stride, size align, size count) {
    assert(stride != 0);
    size padding = -(usize)a->beg & (align - 1);
    size available = a->end - a->beg - padding;
    if (available < 0 || count > available / stride) {
        fprintf(stderr, "Arena OOM\n");
        abort(); //arena is out of memory
    }
    void * p = a->beg + padding;
    a->beg += padding + count * stride;
    return memset(p, 0, count * stride);
}
