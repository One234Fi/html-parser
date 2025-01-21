#include "parser/token.h"
#include "mem/scratch_arena.h"
#include "mem/arena.h"
#include "types/str.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool emit_token(token t) {
    scratch_arena * a = scratch_arena_get();
    string s = token_to_string(t, &a->a);
    s = s_cat(s, make_string("\0"), &a->a);
    fprintf(stdout, "%s", s.data);
    scratch_arena_release(a);
    return true;
}
