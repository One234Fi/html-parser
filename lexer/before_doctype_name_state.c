#include "lexer_internal.h"
#include "lexer.h"


void before_doctype_name_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            // intentionally ignore character
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_current_token(p, token_doctype_init());
            append_to_current_tag_token_name(p, UNICODE_REPLACEMENT_CHAR);
            p->state = DOCTYPE_NAME_STATE;
            break;
        case '>':
            LOG_ERROR(xstr(MISSING_DOCTYPE_NAME_PARSE_ERROR));
            set_current_token(p, token_doctype_init());
            set_doctype_token_force_quirks_flag(p, true);
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_current_token(p, token_doctype_init());
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(p, get_current_token(p));
            emit_token(p, token_eof_init());
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                set_current_token(p, token_doctype_init());
                append_to_current_tag_token_name(p, tolower(c));
                p->state = DOCTYPE_NAME_STATE;
            } else {
                set_current_token(p, token_doctype_init());
                append_to_current_tag_token_name(p, c);
                p->state = DOCTYPE_NAME_STATE;
            }
    }
}
