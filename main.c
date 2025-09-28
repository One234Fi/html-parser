/*
 * Html parser entry point
 * 08/24/2024
 * 09/19/2025 - lol I thought this would be a two week project when I started...
 */


#include "lexer.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "fickit.h"

#include "dumb.h"


int main(int argc, char* argv[]) {
    fprintf(stderr, "Passed %d arguments\n", argc);

    bool parse_links = false;
    char * file_path = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--parse-links") == 0) {
            parse_links = true;
        } else if (!file_path) {
            file_path = argv[i];
        } else if (parse_links && file_path) {
            break;
        }
    }

    if (file_path) {
        char * backing = malloc(1 << 18);
        assert(backing != NULL);
        arena global = arena_wrap(1 << 18, backing);
        arena scratch1 = arena_slice(&global, 1 << 12);
        arena scratch2 = arena_slice(&global, 1 << 12);
        global.debug_name = "global";
        scratch1.debug_name = "stack mem";
        scratch2.debug_name = "string mem";

        lexer lex = lexer_init(file_path, &global);
        node * tree = parse_tree(&lex, &global, scratch1, scratch2);
        if (parse_links) {
            tree_for_each(&tree, print_link, NULL);
        } else {
            tree_for_each(&tree, print_content, NULL);
        }
        fprintf(stderr, "\nEOF\n");
        free(backing);
    }

    return EXIT_SUCCESS;
}


//int old_main(int argc, char* argv[]) {
//    fprintf(stdout, "Passed %d arguments\n", argc);
//
//    if (argc > 1) {
//        char * backing = malloc(1 << 16);
//        assert(backing != NULL);
//        arena global = arena_wrap(1 << 16, backing);
//        parser p = parser_init(argv[1], &global);
//        int c = 0;
//        token t = get_token(&p.lex);
//        while (t.type != END_OF_FILE) {
//            c++;
//            t = get_token(&p.lex);
//        }
//        printf("Token Count: %d\n", c);
//        free(backing);
//    }
//
//    return EXIT_SUCCESS;
//}
