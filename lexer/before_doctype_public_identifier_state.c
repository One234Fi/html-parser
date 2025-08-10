#include "lexer_internal.h"
#include "lexer.h"


void before_doctype_public_identifier_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore character
            break;
        case '"':
            set_current_token_identifier(p, "", 0);
            p->state = DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE;
            break;
        case '\'':
            set_current_token_identifier(p, "", 0);
            p->state = DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE;
            break;
        case '>':
            LOG_ERROR(xstr(MISSING_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(p, get_current_token(p));
            emit_token(p, token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            input_system_reconsume(&p->input);
            p->state = BOGUS_DOCTYPE_STATE;
    }
}
