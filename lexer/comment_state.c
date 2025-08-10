#include "lexer_internal.h"
#include "lexer.h"


void comment_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '<':
            append_to_current_tag_token_comment_data(p, c);
            p->state = COMMENT_LESS_THAN_SIGN_STATE;
            break;
        case '-':
            p->state = COMMENT_END_DASH_STATE;
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_comment_data(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_COMMENT_PARSE_ERROR));
            emit_token(p, get_current_token(p));
            emit_token(p, token_eof_init());
            break;
        default:
            append_to_current_tag_token_comment_data(p, c);
    }
}
