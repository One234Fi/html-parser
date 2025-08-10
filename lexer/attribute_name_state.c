#include "lexer_internal.h"
#include "lexer.h"


void attribute_name_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
        case EOF:
            input_system_reconsume(&p->input);
            p->state = AFTER_ATTRIBUTE_NAME_STATE;
            check_for_duplicate_attributes(p);
            break;
        case '=':
            p->state = BEFORE_ATTRIBUTE_VALUE_STATE;
            check_for_duplicate_attributes(p);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_attribute_name(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case '"':
        case '\'':
        case '<':
            LOG_ERROR(xstr(UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR));
            append_to_current_tag_token_attribute_name(p, c);
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_attribute_name(p, tolower(c));
            } else {
                append_to_current_tag_token_attribute_name(p, c);
            }
    }
}
