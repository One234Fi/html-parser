#include "lexer_internal.h"
#include "lexer.h"


void script_data_escaped_less_than_sign_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '/':
            clear_temporary_buffer(p);
            p->state = SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE;
            break;
        default:
            if (is_ascii_alpha(c)) {
                clear_temporary_buffer(p);
                emit_token(p, token_character_init('<'));
                input_system_reconsume(&p->input);
                p->state = SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE;
            } else {
                emit_token(p, token_character_init('<'));
                input_system_reconsume(&p->input);
                p->state = SCRIPT_DATA_ESCAPED_STATE;
            }
    }
}
