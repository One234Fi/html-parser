/*
 * Html parser entry point
 * 08/24/2024
 */


#include "input.h"
#include "mem/mem.h"
#include "mem/arena.h"
#include "mem/scratch_arena.h"
#include "parser/tokenizer.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    fprintf(stdout, "%d\n", argc);

    if (argc > 1) {
        arena global = arena_init(1 << 16);
        char * tbf = global.beg;
        scratch_space_init();
        input_system s = input_system_init(argv[1], &global);
        parser p = parser_init(&global, s);
        while (!p.eof_emitted) {
            execute(&p);
        }

        scratch_space_destroy();
        xfree(tbf);
    }

    return EXIT_SUCCESS;
}
