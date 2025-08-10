#include "lexer_internal.h"
#include "lexer.h"


void comment_less_than_sign_bang_dash_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            p->state = COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH_STATE;
            break;
        default:
            input_system_reconsume(&p->input);
            p->state = COMMENT_END_DASH_STATE;
    }
}
