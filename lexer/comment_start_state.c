#include "lexer_internal.h"
#include "lexer.h"


void comment_start_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            p->state = COMMENT_START_DASH_STATE;
            break;
        case '>':
            LOG_ERROR(xstr(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR));
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        default:
            input_system_reconsume(&p->input);
            p->state = COMMENT_STATE;
    }
}
