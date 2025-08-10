#include "lexer_internal.h"
#include "lexer.h"


void after_attribute_name_state(lexer * lexer) {
    int c = input_system_consume(&lexer->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore these characters
            break;
        case '/':
            lexer->state = SELF_CLOSING_START_TAG_STATE;
            break;
        case '=':
            lexer->state = BEFORE_ATTRIBUTE_VALUE_STATE;
            break;
        case '>':
            lexer->state = DATA_STATE;
            break;
        case EOF:
            lexer->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(lexer, token_eof_init());
            break;
        default:
            start_new_attribute_for_current_tag_token(lexer);
            input_system_reconsume(&lexer->input);
            lexer->state = ATTRIBUTE_NAME_STATE;
    }
}
