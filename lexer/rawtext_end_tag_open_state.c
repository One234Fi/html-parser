#include "lexer_internal.h"
#include "lexer.h"


void rawtext_end_tag_open_state(lexer * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alpha(c)) {
        set_current_token(p, token_eof_init());
        input_system_reconsume(&p->input);
        p->state = SCRIPT_DATA_END_TAG_NAME_STATE;
    } else {
        emit_token(p, token_character_init('/'));
        input_system_reconsume(&p->input);
        p->state = SCRIPT_DATA_STATE;
    }
}
