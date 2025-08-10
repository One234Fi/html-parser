#include "lexer_internal.h"
#include "lexer.h"


void script_data_double_escaped_less_than_sign_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '/':
            clear_temporary_buffer(p);
            p->state = SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE;
            emit_token(p, token_character_init('/'));
            break;
        default:
            input_system_reconsume(&p->input);
            p->state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
    }
}
