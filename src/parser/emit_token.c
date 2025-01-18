#include "parser/token.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool emit_token(token t) {
    char * s = token_to_string(t);
    if (s != NULL) {
        fprintf(stdout, "%s", s);
        free(s);
    }

    return true;
}
