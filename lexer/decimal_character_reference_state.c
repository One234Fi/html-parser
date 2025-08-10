#include "lexer_internal.h"
#include "lexer.h"


void decimal_character_reference_state(lexer * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_digit(c)) {
        int curr_as_numeric = c - 0x0030;
        int ref_code = p->char_ref_code * 10;
        ref_code += curr_as_numeric;
        p->char_ref_code = ref_code;
    } else if (c == ';') {
        p->state = NUMERIC_CHARACTER_REFERENCE_END_STATE;
    } else {
        LOG_ERROR(xstr(MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE_PARSE_ERROR));
        input_system_reconsume(&p->input);
        p->state = NUMERIC_CHARACTER_REFERENCE_END_STATE;
    }
}
