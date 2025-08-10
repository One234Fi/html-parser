#include "lexer_internal.h"
#include "lexer.h"


void rawtext_less_than_sign_state(lexer * p) {
    int c = input_system_consume(&p->input);
    if (c == '/') {
        clear_temporary_buffer(p);
        p->state = RAWTEXT_END_TAG_OPEN_STATE;
    } else {
        emit_token(p, token_character_init('<'));
        input_system_reconsume(&p->input);
        p->state = RAWTEXT_STATE;
    }
}
