#include "types/opt.h"
#include "types/str.h"
#include "types/arena.h"
#include "types/vector.h"

void opt_str_append(opt_str* s, arena * a, char c) {
    if(!s->exists) {
        s->val = new(a, string);
    }
    *push(((string *) s->val), a) = c;
}
