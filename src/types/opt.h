#ifndef OPTION_MODULE_H
#define OPTION_MODULE_H

#include "mem/arena.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    void * val;
    bool exists;
} opt;

typedef opt opt_str;
typedef opt opt_i32;

#define opt_get(opt, type) ((type *) ((opt)->val))

void opt_str_append(opt_str* s, arena * a, char c);


#endif
