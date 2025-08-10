#include "lexer_internal.h"
#include "lexer.h"


void rawtext_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '<':
            p->state = RAWTEXT_LESS_THAN_SIGN_STATE;
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            emit_token(p, token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true;
            emit_token(p, token_eof_init());
            break;
        default:
            emit_token(p, token_character_init(c));
    }
}
