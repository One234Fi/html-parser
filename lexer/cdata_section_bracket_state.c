#include "lexer_internal.h"
#include "lexer.h"


void cdata_section_bracket_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case ']':
            p->state = CDATA_SECTION_END_STATE;
            break;
        default:
            emit_token(p, token_character_init(']'));
            input_system_reconsume(&p->input);
            p->state = CDATA_SECTION_STATE;
    }
}
