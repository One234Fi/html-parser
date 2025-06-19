#include "opt.h"
#include "str.h"
#include "arena.h"
#include "vector.h"

void opt_str_append(opt_str* s, arena * a, char c) {
    if(!s->exists) {
        s->val = new(a, string);
        s->exists = true;
    }
    *push(opt_get(s, string), a) = c;
}
