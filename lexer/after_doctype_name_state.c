#include "lexer_internal.h"
#include "lexer.h"


void after_doctype_name_state(lexer * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            // intentionally ignore character
            return;
        case '>':
            p->state = DATA_STATE;
            emit_token(p, get_current_token(p));
            return;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(p, get_current_token(p));
            emit_token(p, token_eof_init());
            return;
    }

    string temp = input_system_peekn(&p->input, 5, p->arena);
    char buf[6] = {};
    buf[0] = tolower(c);
    buf[1] = tolower(temp.data[0]);
    buf[2] = tolower(temp.data[1]);
    buf[3] = tolower(temp.data[2]);
    buf[4] = tolower(temp.data[3]);
    buf[5] = tolower(temp.data[4]);

    if (strncmp(buf, "public", 6) == 0) {
        for (size_t i = 0; i < 5; i++) {
            input_system_consume(&p->input);
        }
        p->state = AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE;
        return;
    }  

    if (strncmp(buf, "system", 6) == 0) {
        for (size_t i = 0; i < 5; i++) {
            input_system_consume(&p->input);
        }
        p->state = AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE;
        return;
    }

    LOG_ERROR(xstr(INVALID_CHARACTER_SEQUENCE_AFTER_DOCTYPE_NAME_PARSE_ERROR));
    set_doctype_token_force_quirks_flag(p, true);
    input_system_reconsume(&p->input);
    p->state = BOGUS_DOCTYPE_STATE;
}
