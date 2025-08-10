#include "lexer_internal.h"
#include "lexer.h"


void doctype_public_identifier_double_quoted_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '"':
            p->state = AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE;
            break;
        case '>':
            LOG_ERROR(xstr(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(p, get_current_token(p));
            break;
        default:
            append_to_current_tag_token_identifier(p, c);
    }
}
