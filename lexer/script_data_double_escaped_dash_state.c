#include "lexer_internal.h"
#include "lexer.h"


void script_data_double_escaped_dash_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '-':
            p->state = SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE;
            emit_token(p, token_character_init('-'));
            break;
        case '<':
            p->state = SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE;
            emit_token(p, token_character_init('<'));
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            p->state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
            emit_token(p, token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            emit_token(p, token_eof_init());
            break;
        default:
            p->state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
            emit_token(p, token_character_init(c));
    }
}
