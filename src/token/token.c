#include <stddef.h>
#include <stdint.h>
#include "optional/optional.h"
#include "token/token.h"

token * token_doctype_init(arena * a) {
    token_doctype * t = new(a, token_doctype);
    t->token.type = DOCTYPE;
    opt_init(t->name, vec_char, a);
    opt_init(t->public_id, int32_t, a);
    opt_init(t->system_id, int32_t, a);
    return &t->token;
}

token * token_start_tag_init(arena * a) {
    token_start_tag * t = new(a, token_start_tag);
    t->token.type = START_TAG;
    opt_init(t->token_tag.name, vec_char, a);
    return &t->token;
}

token * token_end_tag_init(arena * a) {
    token_end_tag * t = new(a, token_end_tag);
    t->token.type = END_TAG;
    opt_init(t->token_tag.name, vec_char, a);
    return &t->token;
}

token * token_comment_init(arena * a) {
    token_comment * t = new(a, token_comment);
    t->token.type = COMMENT;
    return &t->token;
}

token * token_character_init(arena * a, char c) {
    token_character * t = new(a, token_character);
    t->token.type = CHARACTER;
    t->data = c;
    return &t->token;
}

token * token_eof_init(arena * a) {
    token_eof * t = new(a, token_eof);
    t->token.type = END_OF_FILE;
    return &t->token;
}
