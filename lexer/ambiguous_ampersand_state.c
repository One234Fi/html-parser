#include "lexer_internal.h"
#include "lexer.h"


void ambiguous_ampersand_state(lexer * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alphanumeric(c)) {
        if (is_part_of_an_attribute(p)) {
            append_to_current_tag_token_attribute_value(p, c);
        } else {
            emit_token(p, token_character_init(c));
        }
    } else if (c == ';') {
        LOG_ERROR(xstr(UNKNOWN_NAMED_CHARACTER_REFERENCE_PARSE_ERROR));
        input_system_reconsume(&p->input);
        p->state = p->return_state;
    } else {
        input_system_reconsume(&p->input);
        p->state = p->return_state;
    }
}
