#include "lexer_internal.h"
#include "lexer.h"


void numeric_character_reference_state(lexer * p) {
    p->char_ref_code = 0;
    int c = input_system_consume(&p->input);
    switch (c) {
        case 'x':
        case 'X':
            append_to_temp_buffer(p, c);
            p->state = HEXADECIMAL_CHARACTER_REFERENCE_START_STATE;
            break;
        default:
            input_system_reconsume(&p->input);
            p->state = DECIMAL_CHARACTER_REFERENCE_START_STATE;
    }
}
