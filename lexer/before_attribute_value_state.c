#include "lexer_internal.h"
#include "lexer.h"


void before_attribute_value_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore these characters
            break;
        case '"':
            p->state = ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE;
            break;
        case '\'':
            p->state = ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE;
            break;
        case '>':
            LOG_ERROR(xstr(MISSING_ATTRIBUTE_VALUE_PARSE_ERROR));
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        default:
            input_system_reconsume(&p->input);
            p->state = ATTRIBUTE_VALUE_UNQUOTED_STATE;
    }
}
