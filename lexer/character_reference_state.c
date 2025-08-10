#include "lexer_internal.h"
#include "lexer.h"


void character_reference_state(lexer * p) {
    clear_temporary_buffer(p);
    append_to_temp_buffer(p, '&');
    int c = input_system_consume(&p->input);
    switch (c) {
        case '#':
            append_to_temp_buffer(p, c);
            p->state = NUMERIC_CHARACTER_REFERENCE_STATE;
            break;
        default:
            if (is_ascii_alphanumeric(c)) {
                input_system_reconsume(&p->input);
                p->state = NAMED_CHARACTER_REFERENCE_STATE;
            } else {
                flush_code_points(p);
                input_system_reconsume(&p->input);
                p->state = p->return_state;
            }
    }
}
