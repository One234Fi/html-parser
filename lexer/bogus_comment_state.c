#include "lexer_internal.h"
#include "lexer.h"


void bogus_comment_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            emit_token(p, get_current_token(p));
            emit_token(p, token_eof_init());
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_comment_data(p, UNICODE_REPLACEMENT_CHAR);
            break;
        default:
            append_to_current_tag_token_comment_data(p, c);
    }
}
