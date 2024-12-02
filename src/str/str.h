#ifndef STRING_MODULE_H
#define STRING_MODULE_H

#include <stddef.h>

#define make_string(s) (string){(char*)s, sizeof(s)-1}

typedef struct {
    char * data;
    ptrdiff_t len;
} string;

#endif
