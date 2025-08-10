#include "lexer_internal.h"
#include "lexer.h"


void data_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '&': 
            p->return_state = DATA_STATE; 
            p->state = CHARACTER_REFERENCE_STATE;
            break;
        case '<': 
            p->state = TAG_OPEN_STATE; 
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            emit_token(p, token_character_init(c));
            break;
        case EOF:
		    p->eof_emitted = true;
            emit_token(p, token_eof_init());
            break;
        default:
            emit_token(p, token_character_init(c));
    }
}
