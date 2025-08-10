/*
 * Html parser entry point
 * 08/24/2024
 */


#include "parser.h"
#include "lexer.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#define FICKIT_IMPL
#include "fickit.h"

int main(int argc, char* argv[]) {
    fprintf(stdout, "Passed %d arguments\n", argc);

    if (argc > 1) {
        char * backing = malloc(1 << 16);
        assert(backing != NULL);
        arena global = arena_wrap(1 << 16, backing);
        parser p = parser_init(argv[1], &global);
        int c = 0;
        token t = get_token(&p.lex);
        while (t.type != END_OF_FILE) {
            c++;
            t = get_token(&p.lex);
        }
        printf("Token Count: %d\n", c);
        free(backing);
    }

    return EXIT_SUCCESS;
}
