#include "mem/scratch_arena.h"
#include "mem/mem.h"
#include <stdlib.h>

//TODO: make get/rel threadsafe

typedef struct {
    arena backing;
    scratch_arena * free_head;
    char * tbf;
} scratch_space;

static scratch_space global = {0};

void scratch_space_init() {
    global.backing = arena_init(sizeof(arena) * 8);
    global.tbf = global.backing.beg;
}

void scratch_space_destroy() {
    for (scratch_arena * n = global.free_head; n != NULL; n = n->next) {
        //xfree(n->pos);
    }
    //xfree(global.tbf);
}

scratch_arena * scratch_arena_get() { 
    scratch_arena * s = global.free_head;
    if (s) {
        global.free_head = global.free_head->next;
        return s;
    } else {
        s = new(&global.backing, scratch_arena);
        s->a = arena_init(1024);
        s->pos = s->a.beg;
        return s;
    }
}

void scratch_arena_release(scratch_arena * scratch) {
    scratch->a.beg = scratch->pos;
    scratch->next = global.free_head;
    global.free_head = scratch;
}
