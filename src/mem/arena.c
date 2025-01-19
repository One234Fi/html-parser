#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "mem/arena.h"
#include "common.h"
#include "mem/mem.h"


arena arena_init(ptrdiff_t cap) {
    arena a = {0};
    a.beg = xmalloc(cap);
    a.end = a.beg + cap;
    return a;
}

void * alloc(arena * a, size stride, size align, size count) {
    assert(stride != 0);
    size padding = -(usize)a->beg & (align - 1);
    size available = a->end - a->beg - padding;
    if (available < 0 || count > available / stride) {
        die("Arena OOM");
    }
    void * p = a->beg + padding;
    a->beg += padding + count * stride;
    return memset(p, 0, count * stride);
}
