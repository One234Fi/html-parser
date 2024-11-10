#include "common.h"
#include <stdlib.h>
#include "mem.h"

void * MALLOC(const size_t size) {
    void * p = malloc(size);
    if (!p) {
        LOG_ERROR("MALLOC FAILED");
        exit(EXIT_FAILURE);
    }
    return p;
}

void * REALLOC(void * p, const size_t size) {
    void * newptr = realloc(p, size);
    if (!newptr) {
        LOG_ERROR("REALLOC FAILED");
        exit(EXIT_FAILURE);
    }
    return newptr;
}
