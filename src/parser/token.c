#include <stddef.h>
#include <stdint.h>
#include "parser/token.h"

token token_init(token_type type) {
    token t = {0};
    t.type = type;
    return t;
}

token token_doctype_init() {
    return token_init(DOCTYPE);
}

token token_start_tag_init() {
    return token_init(START_TAG);
}

token token_end_tag_init() {
    return token_init(END_TAG);
}

token token_comment_init() {
    return token_init(COMMENT);
}

token token_character_init(char c) {
    token t = token_init(CHARACTER);
    t.character.data = c;
    return t;
}

token token_eof_init() {
    return token_init(END_OF_FILE);
}
