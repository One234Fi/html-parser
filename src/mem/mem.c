#include "mem.h"
#include "common.h"
#include <assert.h>
#include <stdlib.h>

void * xmalloc(size s) {
    void * m = malloc(s);
    if (!m) {
        die("Malloc Failed");
    }
    return m;
}

void * xrealloc(void * ptr, size s) {
    assert(s > 0);
    if (!ptr) {
        return xmalloc(s);
    }

    void * m = realloc(ptr, s);
    if (!m) {
        die("Realloc Failed");
    }
    return m;
}
