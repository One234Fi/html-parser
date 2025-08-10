#include "lexer_internal.h"
#include "lexer.h"


void cdata_section_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case ']':
            p->state = CDATA_SECTION_BRACKET_STATE;
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_CDATA_PARSE_ERROR));
            emit_token(p, token_eof_init());
            break;
        default:
            emit_token(p, token_character_init(c));
    }
}
