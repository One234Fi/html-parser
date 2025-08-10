#include "lexer_internal.h"
#include "lexer.h"


void cdata_section_end_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case ']':
            emit_token(p, token_character_init(']'));
            break;
        case '>':
            p->state = DATA_STATE;
            break;
        default:
            emit_token(p, token_character_init(']'));
            emit_token(p, token_character_init(']'));
            input_system_reconsume(&p->input);
            p->state = CDATA_SECTION_STATE;
    }
}
