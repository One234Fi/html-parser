#include "arena.h"
#include "token.h"
#include "tokenizer.h"
#include <stdio.h>

int main() {
    arena a = arena_init(2<<14);
    arena scratch = arena_init(2<<8);
    parser p = parser_init("testdata/short.html", &a);

    while (!p.eof_emitted) {
        token t = get_token(&p);
        arena s = scratch;
        string token_string = token_to_string(t, &s);
        printf("%.*s", (int) token_string.len, token_string.data);
    }
}
