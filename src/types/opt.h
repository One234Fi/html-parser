#ifndef OPTION_MODULE_H
#define OPTION_MODULE_H

#include "types/str.h"
#include "types/types.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    union {
        string string;
        i32 i32;
    };
    bool exists;
} opt;

typedef opt opt_str;
typedef opt opt_i32;

#endif
