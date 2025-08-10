#include "lexer_internal.h"
#include "lexer.h"


void script_data_double_escape_start_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
            if (strncmp(get_temporary_buffer(p), "script", 6) == 0) {
                p->state = SCRIPT_DATA_DOUBLE_ESCAPED_STATE;
            } else {
                p->state = SCRIPT_DATA_ESCAPED_STATE;
            }
            emit_token(p, token_character_init(c));
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_temp_buffer(p, tolower(c));
                emit_token(p, token_character_init(c));
            } else if (is_ascii_lower_alpha(c)) {
                append_to_temp_buffer(p, c);
                emit_token(p, token_character_init(c));
            } else {
                input_system_reconsume(&p->input);
                p->state = SCRIPT_DATA_ESCAPED_STATE;
            }
    }
}
