#ifndef HTML_TOKENIZER_MODULE
#define HTML_TOKENIZER_MODULE

/*
 * Types used by the tokenizer
 * 08/24/2024
 */

#include "input.h"
#include "mem/arena.h"
#include "parser/token.h"
#include "types/enum.h"

#define DEFINE_STATE_TYPES(S) \
    S(DATA_STATE) \
    S(RCDATA_STATE)  \
    S(RAWTEXT_STATE) \
    S(SCRIPT_DATA_STATE) \
    S(PLAINTEXT_STATE) \
    S(TAG_OPEN_STATE) \
    S(END_TAG_OPEN_STATE) \
    S(TAG_NAME_STATE) \
    S(RCDATA_LESS_THAN_SIGN_STATE) \
    S(RCDATA_END_TAG_OPEN_STATE) \
    S(RCDATA_END_TAG_NAME_STATE) \
    S(RAWTEXT_LESS_THAN_SIGN_STATE)  \
    S(RAWTEXT_END_TAG_OPEN_STATE) \
    S(RAWTEXT_END_TAG_NAME_STATE) \
    S(SCRIPT_DATA_LESS_THAN_SIGN_STATE)  \
    S(SCRIPT_DATA_END_TAG_OPEN_STATE) \
    S(SCRIPT_DATA_END_TAG_NAME_STATE) \
    S(SCRIPT_DATA_ESCAPE_START_STATE) \
    S(SCRIPT_DATA_ESCAPE_START_DASH_STATE) \
    S(SCRIPT_DATA_ESCAPED_STATE) \
    S(SCRIPT_DATA_ESCAPED_DASH_STATE) \
    S(SCRIPT_DATA_ESCAPED_DASH_DASH_STATE) \
    S(SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE)  \
    S(SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE) \
    S(SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE) \
    S(SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE) \
    S(SCRIPT_DATA_DOUBLE_ESCAPED_STATE)  \
    S(SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE) \
    S(SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE) \
    S(SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE) \
    S(SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE) \
    S(BEFORE_ATTRIBUTE_NAME_STATE) \
    S(ATTRIBUTE_NAME_STATE)  \
    S(AFTER_ATTRIBUTE_NAME_STATE) \
    S(BEFORE_ATTRIBUTE_VALUE_STATE)  \
    S(ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE) \
    S(ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE) \
    S(ATTRIBUTE_VALUE_UNQUOTED_STATE) \
    S(AFTER_ATTRIBUTE_VALUE_QUOTED_STATE) \
    S(SELF_CLOSING_START_TAG_STATE)  \
    S(BOGUS_COMMENT_STATE) \
    S(MARKUP_DECLARATION_OPEN_STATE) \
    S(COMMENT_START_STATE) \
    S(COMMENT_START_DASH_STATE)  \
    S(COMMENT_STATE) \
    S(COMMENT_LESS_THAN_SIGN_STATE)  \
    S(COMMENT_LESS_THAN_SIGN_BANG_STATE) \
    S(COMMENT_LESS_THAN_SIGN_BANG_DASH_STATE) \
    S(COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH_STATE) \
    S(COMMENT_END_DASH_STATE) \
    S(COMMENT_END_STATE) \
    S(COMMENT_END_BANG_STATE) \
    S(DOCTYPE_STATE) \
    S(BEFORE_DOCTYPE_NAME_STATE) \
    S(DOCTYPE_NAME_STATE) \
    S(AFTER_DOCTYPE_NAME_STATE)  \
    S(AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE) \
    S(BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE) \
    S(DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE) \
    S(DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE) \
    S(AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE) \
    S(BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE) \
    S(AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE) \
    S(BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE) \
    S(DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE) \
    S(DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE) \
    S(AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE) \
    S(BOGUS_DOCTYPE_STATE) \
    S(CDATA_SECTION_STATE) \
    S(CDATA_SECTION_BRACKET_STATE) \
    S(CDATA_SECTION_END_STATE) \
    S(CHARACTER_REFERENCE_STATE) \
    S(NAMED_CHARACTER_REFERENCE_STATE) \
    S(AMBIGUOUS_AMPERSAND_STATE) \
    S(NUMERIC_CHARACTER_REFERENCE_STATE) \
    S(HEXADECIMAL_CHARACTER_REFERENCE_START_STATE) \
    S(DECIMAL_CHARACTER_REFERENCE_START_STATE) \
    S(HEXADECIMAL_CHARACTER_REFERENCE_STATE) \
    S(DECIMAL_CHARACTER_REFERENCE_STATE) \
    S(NUMERIC_CHARACTER_REFERENCE_END_STATE) \
    S(INVALID_TOKENIZER_STATE)

enum TOKENIZER_STATE_TYPE {
    DEFINE_STATE_TYPES(DEFINE_ENUM)
};

extern const char * TOKENIZER_STATE_STRINGS[];

enum INSERTION_MODE_TYPE {
    INSERTION_MODE_INITIAL,
    INSERTION_MODE_BEFORE_HTML,
    INSERTION_MODE_BEFORE_HEAD,
    INSERTION_MODE_IN_HEAD,
    INSERTION_MODE_IN_HEAD_NOSCRIPT,
    INSERTION_MODE_AFTER_HEAD,
    INSERTION_MODE_IN_BODY,
    INSERTION_MODE_TEXT,
    INSERTION_MODE_IN_TABLE,
    INSERTION_MODE_IN_TABLE_TEXT,
    INSERTION_MODE_IN_CAPTION,
    INSERTION_MODE_IN_COLUMN_GROUP,
    INSERTION_MODE_IN_TABLE_BODY,
    INSERTION_MODE_IN_ROW,
    INSERTION_MODE_IN_CELL,
    INSERTION_MODE_IN_SELECT,
    INSERTION_MODE_IN_SELECT_IN_TABLE,
    INSERTION_MODE_IN_TEMPLATE,
    INSERTION_MODE_AFTER_BODY,
    INSERTION_MODE_IN_FRAMESET,
    INSERTION_MODE_AFTER_FRAMESET,
    INSERTION_MODE_AFTER_AFTER_BODY,
    INSERTION_MODE_AFTER_AFTER_FRAMESET,
};

typedef struct parser parser;
struct parser {
    token current_token;
    enum TOKENIZER_STATE_TYPE state;
    enum TOKENIZER_STATE_TYPE return_state;
    enum INSERTION_MODE_TYPE insert_mode;
    bool parser_pause_flag;
    bool eof_emitted;
    opt_str last_start_tag_name;
    arena * arena;
    input_system input;
    string temp_buf;
    int char_ref_code;
};

void execute(parser * p);
parser parser_init(arena * a, input_system i);

#endif
