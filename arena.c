#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "arena.h"
#include "common.h"


arena arena_init(ptrdiff_t cap) {
    arena a = {0};
    a.beg = malloc(cap);
    a.pos = a.beg;
    a.end = a.beg ? a.beg + cap : 0;
    return a;
}

arena arena_wrap(ptrdiff_t cap, void * mem) {
    arena a = {0};
    a.beg = mem;
    a.pos = a.beg;
    a.end = a.beg + cap;
    return a;
}

void * alloc(arena * a, size stride, size align, size count) {
    assert(stride != 0);
    size padding = -(usize)a->pos & (align - 1);
    size available = a->end - a->pos - padding;
    if (available < 0 || count > available / stride) {
        die("Arena OOM");
    }
    void * p = a->pos + padding;
    a->pos += padding + count * stride;
    return memset(p, 0, count * stride);
}


