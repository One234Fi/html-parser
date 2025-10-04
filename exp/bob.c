//bob can build it
// cc bob.c -o bob.out
// ./bob.out
#include <stdlib.h>
#include <string.h>
#define FICKIT_IMPL
#include "fickit.h"

int main(int argc, char * argv[]) {
    char * cc = "/bin/gcc";
    char * cc_flags[] = {
        "-std=c23",
        "-g3",
        "-D_POSIX_C_SOURCE",
        "-Wall",
        "-Wextra",
		"-Wno-unused-parameter",
        "-Werror=return-type",
		"-fsanitize=undefined",
        "-fsanitize-recover=undefined",
        NULL
    };
    bob_bootstrap(argv[0], cc, cc_flags);

    arena a = arena_init(4096 * 16);

    command c = {0};
    *push(&c, &a) = cc;
    for_each(cc_flags, pflag) { *push(&c, &a) = *pflag; };
    *push(&c, &a) = "./main.c";
    *push(&c, &a) = "-o";
    *push(&c, &a) = "./ark";

    run(&c, NULL, NULL);
}

