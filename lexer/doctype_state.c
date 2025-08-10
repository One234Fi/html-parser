#include "lexer_internal.h"
#include "lexer.h"


void doctype_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            p->state = BEFORE_DOCTYPE_NAME_STATE;
            break;
        case '>':
            input_system_reconsume(&p->input);
            p->state = BEFORE_DOCTYPE_NAME_STATE;
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_COMMENT_PARSE_ERROR));
            set_current_token(p, token_doctype_init());
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(p, get_current_token(p));
            emit_token(p, token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_WHITESPACE_BEFORE_DOCTYPE_NAME_PARSE_ERROR));
            input_system_reconsume(&p->input);
            p->state = BEFORE_DOCTYPE_NAME_STATE;
    }
}
