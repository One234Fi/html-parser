#include "tokenizer.h"
#include "enum.h"


const char * TOKENIZER_STATE_STRINGS[INVALID_TOKENIZER_STATE + 1] = {
    DEFINE_STATE_TYPES(MAKE_ENUM_STRINGS)
};
