#include "mem.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "option.h"

opt *
opt_init(const size_t size) {
    opt * p = MALLOC(sizeof(opt));
    p->data = MALLOC(size);
    p->has_data = false;
    return p;
}

void
opt_free(opt * p) {
    FREE(p->data);
    FREE(p);
}
