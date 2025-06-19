/*
 * Html parser entry point
 * 08/24/2024
 */


#include "arena.h"
#include "tokenizer.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    fprintf(stdout, "%d\n", argc);

    if (argc > 1) {
        char * backing = malloc(1 << 16);
        assert(backing != NULL);
        arena global = arena_wrap(1 << 16, backing);
        parser p = parser_init(argv[1], &global);
        while (!p.eof_emitted) {
            execute(&p);
        }
        free(backing);
    }

    return EXIT_SUCCESS;
}
