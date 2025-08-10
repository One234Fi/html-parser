#include "lexer_internal.h"
#include "lexer.h"


void invalid_state(lexer * p) {
    LOG_ERROR(xstr(INVALID_TOKENIZER_STATE_ERROR));
}
