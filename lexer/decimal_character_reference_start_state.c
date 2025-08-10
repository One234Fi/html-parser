#include "lexer_internal.h"
#include "lexer.h"


void decimal_character_reference_start_state(lexer * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_digit(c)) {
        input_system_reconsume(&p->input);
        p->state = DECIMAL_CHARACTER_REFERENCE_STATE;
    } else {
        LOG_ERROR(xstr(ABSENCE_OF_DIGITS_IN_NUMERIC_CHARACTER_REFERENCE_PARSE_ERROR));
        flush_code_points(p);
        input_system_reconsume(&p->input);
        p->state = p->return_state;
    }
}
