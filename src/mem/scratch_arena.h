#ifndef SCRATCH_ARENA_MODULE_H
#define SCRATCH_ARENA_MODULE_H

#include "mem/arena.h"

typedef struct scratch_arena scratch_arena;
struct scratch_arena {
    arena a;
    char * pos;
    scratch_arena * next;
};

void scratch_space_init();
void scratch_space_destroy();


scratch_arena * scratch_arena_get();
void scratch_arena_release(scratch_arena * scratch);

#endif
