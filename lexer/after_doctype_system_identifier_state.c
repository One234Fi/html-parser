#include "lexer_internal.h"
#include "lexer.h"


void after_doctype_system_identifier_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore character
            break;
        case '>':
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        default:
            LOG_ERROR(xstr(UNEXPECTED_CHARACTER_AFTER_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            input_system_reconsume(&p->input);
            p->state = BOGUS_DOCTYPE_STATE;
    }
}
