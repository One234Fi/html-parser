#include "lexer_internal.h"
#include "lexer.h"


void comment_end_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        case '!':
            p->state = COMMENT_END_BANG_STATE;
            break;
        case '-':
            append_to_current_tag_token_comment_data(p, '-');
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_COMMENT_PARSE_ERROR));
            emit_token(p, get_current_token(p));
            emit_token(p, token_eof_init());
            break;
        default:
            append_to_current_tag_token_comment_data(p, '-');
            append_to_current_tag_token_comment_data(p, '-');
            input_system_reconsume(&p->input);
            p->state = COMMENT_STATE;
    }
}
