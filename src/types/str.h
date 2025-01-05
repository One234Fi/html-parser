#ifndef STRING_MODULE_H
#define STRING_MODULE_H

#include "types/types.h"
#include <stdbool.h>
#include <stddef.h>

#define make_string(s) (string){(char*)s, sizeof(s)-1, 0}

typedef struct {
    char * data;
    size len;
    size cap;
} string;

bool string_equal(string a, string b);


#endif
