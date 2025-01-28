#ifndef STRING_MODULE_H
#define STRING_MODULE_H

#include "mem/arena.h"
#include "types/types.h"
#include <stdbool.h>
#include <stddef.h>

#define String(s) (string){(char*)s, sizeof(s)-1, 0}

typedef struct {
    char * data;
    size len;
    size cap;
} string;

bool s_equal(string a, string b);
bool s_equal_ignore_case(string a, string b, arena scratch);
string s_to_lower(string a);
string s_to_upper(string a);
string s_clone(string s, arena * a);
string s_cat(string a, string b, arena * perm);


#endif
