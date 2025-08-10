#include "lexer_internal.h"
#include "lexer.h"


void bogus_doctype_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            p->state = DATA_STATE;
            emit_token(p, token_doctype_init());
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            // intentionally ignore character
            break;
        case EOF:
            p->eof_emitted = true;
            emit_token(p, get_current_token(p));
            emit_token(p, token_eof_init());
            break;
            // otherwise intentionally ignore character
    }
}
