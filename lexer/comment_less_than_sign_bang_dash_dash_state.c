#include "lexer_internal.h"
#include "lexer.h"


void comment_less_than_sign_bang_dash_dash_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
        case EOF:
            input_system_reconsume(&p->input);
            p->state = COMMENT_END_STATE;
            break;
        default:
            LOG_ERROR(xstr(NESTED_COMMENT_PARSE_ERROR));
            input_system_reconsume(&p->input);
            p->state = COMMENT_END_STATE;
    }
}
