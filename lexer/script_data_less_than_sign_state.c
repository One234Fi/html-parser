#include "lexer_internal.h"
#include "lexer.h"


void script_data_less_than_sign_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '/':
            clear_temporary_buffer(p);
            p->state = SCRIPT_DATA_END_TAG_OPEN_STATE;
            break;
        case '!':
            p->state = SCRIPT_DATA_ESCAPE_START_STATE;
            emit_token(p, token_character_init('<'));
            emit_token(p, token_character_init('!'));
            break;
        default:
            emit_token(p, token_character_init('<'));
            p->state = SCRIPT_DATA_STATE;
    }
}
