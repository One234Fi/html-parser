#include "lexer_internal.h"
#include "lexer.h"


void after_attribute_value_quoted_state(lexer * lexer) {
    int c = input_system_consume(&lexer->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            lexer->state = BEFORE_ATTRIBUTE_NAME_STATE;
            break;
        case '/':
            lexer->state = SELF_CLOSING_START_TAG_STATE;
            break;
        case EOF:
            lexer->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(lexer, token_eof_init());
            break;
        case '>':
            lexer->state = DATA_STATE;
            emit_token(lexer, lexer->current_token);
            break;
        default:
            LOG_ERROR(xstr(MISSING_WHITESPACE_BETWEEN_ATTRIBUTES_PARSE_ERROR));
            input_system_reconsume(&lexer->input);
            lexer->state = BEFORE_ATTRIBUTE_NAME_STATE;
    }
}
