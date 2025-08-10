#include "lexer_internal.h"
#include "lexer.h"


void attribute_value_double_quoted_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '"':
            p->state = AFTER_ATTRIBUTE_VALUE_QUOTED_STATE;
            break;
        case '&':
            p->return_state = ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE;
            p->state = CHARACTER_REFERENCE_STATE;
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(p, token_eof_init());
            break;
        default:
            append_to_current_tag_token_attribute_value(p, c);
    }
}
