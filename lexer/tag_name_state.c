#include "lexer_internal.h"
#include "lexer.h"


void tag_name_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            p->state = BEFORE_ATTRIBUTE_NAME_STATE;
            break;
        case '/':
            p->state = SELF_CLOSING_START_TAG_STATE;
            break;
        case '>':
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p)); //emit current tag token
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_name(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(p, token_eof_init());
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(p, tolower(c));
            } else {
                append_to_current_tag_token_name(p, c);
            }
    }
}
