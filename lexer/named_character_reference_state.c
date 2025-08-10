#include "lexer_internal.h"
#include "lexer.h"


void named_character_reference_state(lexer * p) {
    while (is_named_character(input_system_peek(&p->input))) {
        int c = input_system_consume(&p->input);
        append_to_temp_buffer(p, c);

        if (is_part_of_an_attribute(p) 
                && c != ';' 
                && !isalnum(c)
                && (input_system_peek(&p->input) == '=' || isalnum(input_system_peek(&p->input)) )) {
            flush_code_points(p);
            p->state = p->return_state;
            return;
        } else {
            if (c != ';') {
                LOG_ERROR(xstr(MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE_PARSE_ERROR));
            }
            clear_temporary_buffer(p);
            //TODO
            //interpret_character_reference_name(p, c);
            flush_code_points(p);
            p->state = p->return_state;
            return;
        }
    }
    flush_code_points(p);
    p->state = AMBIGUOUS_AMPERSAND_STATE;
}
