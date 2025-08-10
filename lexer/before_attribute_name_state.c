#include "lexer_internal.h"
#include "lexer.h"


void before_attribute_name_state(lexer * p) {
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
            break;
        case '=':
            LOG_ERROR(xstr(UNEXPECTED_EQUALS_SIGN_BEFORE_ATTRIBUTE_NAME_PARSE_ERROR));
            start_new_attribute_for_current_tag_token(p);
            append_to_current_tag_token_attribute_name(p, c);
            p->state = ATTRIBUTE_NAME_STATE;
            break;
        default:
            start_new_attribute_for_current_tag_token(p);
            input_system_reconsume(&p->input);
            p->state = ATTRIBUTE_NAME_STATE;
    }
}
