#include "lexer_internal.h"
#include "lexer.h"


void attribute_value_unquoted_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            p->state = BEFORE_ATTRIBUTE_NAME_STATE;
            break;
        case '&':
            p->return_state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
            p->state = CHARACTER_REFERENCE_STATE;
            break;
        case '>':
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case '"':
        case '\'':
        case '<':
        case '=':
        case '`':
            LOG_ERROR(xstr(UNEXPECTED_CHARACTER_IN_UNQUOTED_ATTRIBUTE_VALUE_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(p, c);
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
