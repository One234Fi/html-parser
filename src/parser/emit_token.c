#include "parser/token.h"
#include "mem/scratch_arena.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool emit_token(token t) {
    scratch_arena * scratch = scratch_arena_get();
    char * s = token_to_string(t, &scratch->a);
    if (s != NULL) {
        fprintf(stdout, "%s", s);
    }
    scratch_arena_release(scratch);
    return true;
}
