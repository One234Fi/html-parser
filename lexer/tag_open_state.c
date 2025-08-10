#include "lexer_internal.h"
#include "lexer.h"


void tag_open_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '!':
            p->state = MARKUP_DECLARATION_OPEN_STATE;
            break;
        case '/':
            p->state = END_TAG_OPEN_STATE;
            break;
        case '?':
            LOG_ERROR(xstr(UNEXPECTED_QUESTION_MARK_INSTEAD_OF_TAG_NAME_PARSE_ERROR));
            set_current_token(p, token_comment_init());
            input_system_reconsume(&p->input);
            p->state = BOGUS_COMMENT_STATE;
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_BEFORE_TAG_NAME_PARSE_ERROR));
            emit_token(p, token_character_init('<'));
            emit_token(p, token_eof_init());
            break;
        default:
            if (is_ascii_alpha(c)) {
                set_current_token(p, token_start_tag_init());
                input_system_reconsume(&p->input);
                p->state = TAG_NAME_STATE;
                return;
            } else {
                LOG_ERROR(xstr(INVALID_FIRST_CHARACTER_OF_TAG_NAME_PARSE_ERROR));
                emit_token(p, token_character_init('<'));
                input_system_reconsume(&p->input);
                p->state = DATA_STATE;
            }
    }
}
