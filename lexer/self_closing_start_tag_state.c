#include "lexer_internal.h"
#include "lexer.h"


void self_closing_start_tag_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            set_self_closing_tag_for_current_token(p, true);
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(p, token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(UNEXPECTED_SOLIDUS_IN_TAG_PARSE_ERROR));
            input_system_reconsume(&p->input);
            p->state = BEFORE_ATTRIBUTE_NAME_STATE;
    }
}
