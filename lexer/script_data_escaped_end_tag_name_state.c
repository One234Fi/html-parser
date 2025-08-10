#include "lexer_internal.h"
#include "lexer.h"


void script_data_escaped_end_tag_name_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid(p)) {
                p->state = BEFORE_ATTRIBUTE_NAME_STATE;
            } else {
                emit_token(p, token_character_init('<'));
                emit_token(p, token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                p->state = SCRIPT_DATA_ESCAPED_STATE;
            }
            break;
        case '/':
            if (current_token_is_valid(p)) {
                p->state = SELF_CLOSING_START_TAG_STATE;
            } else {
                emit_token(p, token_character_init('<'));
                emit_token(p, token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                p->state = SCRIPT_DATA_ESCAPED_STATE;
            }
            break;
        case '>':
            if (current_token_is_valid(p)) {
                p->state = DATA_STATE;
            } else {
                emit_token(p, token_character_init('<'));
                emit_token(p, token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                p->state = SCRIPT_DATA_ESCAPED_STATE;
            }
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(p, tolower(c));
                append_to_temp_buffer(p, c);
            } else if (is_ascii_lower_alpha(c)) {
                append_to_current_tag_token_name(p, c);
                append_to_temp_buffer(p, c);
            } else {
                emit_token(p, token_character_init('<'));
                emit_token(p, token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                p->state = SCRIPT_DATA_ESCAPED_STATE;
            }
    }
}
