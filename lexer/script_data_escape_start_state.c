#include "lexer_internal.h"
#include "lexer.h"


void script_data_escape_start_state(lexer * p) {
    int c = input_system_consume(&p->input);
    if (c == '-') {
        p->state = SCRIPT_DATA_ESCAPE_START_DASH_STATE;
        emit_token(p, token_character_init('-'));
    } else {
        input_system_reconsume(&p->input);
        p->state = SCRIPT_DATA_STATE;
    }
}
