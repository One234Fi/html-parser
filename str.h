#ifndef STRING_MODULE_H
#define STRING_MODULE_H

#include "arena.h"
#include "types.h"
#include <stdbool.h>
#include <stddef.h>

#define String(s) (string){(char*)s, sizeof(s)-1, 0}

typedef struct {
    char * data;
    size len;
    size cap;
} string;

bool s_equal(string a, string b);
bool s_equal_ignore_case(string a, string b);
size s_has_char(string a, char c);
string s_clone(string s, arena * a);
string s_cat(string a, string b, arena * perm);


#endif
