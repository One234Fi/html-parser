#ifndef LEXER_H
#define LEXER_H

#include "fickit.h"

typedef enum TOKEN_TYPE {
    DOCTYPE,
    START_TAG,
    END_TAG,
    COMMENT,
    CHARACTER,
    END_OF_FILE,
    TOKEN_TYPE_COUNT,
} token_type;

typedef struct {
    opt_str name;
    opt_str public_id;
    opt_str system_id;
    bool force_quirks;
} t_doctype;

typedef struct {
    string name;
    string value;
} token_attr;

typedef struct {
    token_attr * data;
    size len;
    size cap;
} token_attrs;

typedef struct {
    string name;
    bool self_closing;
} t_tag;

typedef struct {
    string data;
} t_comment;

typedef struct {
    char data;
} t_character;

typedef struct {
} t_eof;

typedef struct {
    union {
        t_doctype doctype;
        t_tag tag;
        t_comment comment;
        t_character character;
        t_eof eof;
    };
    token_attrs attrs;
    token_type type;
} token;

typedef struct {
    token * data;
    size len;
    size cap;
} tokens;

typedef struct input_system {
    string buffer;
    char * front;
    size len; //stack length
} input_system;

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

enum LEXER_STATE {
    DEFINE_STATE_TYPES(DEFINE_ENUM)
};

typedef struct {
    token current_token;
    tokens emitted_tokens;
    opt_str last_start_tag_name;
    arena * arena;
    input_system input;
    string temp_buf;
    int char_ref_code;
    enum LEXER_STATE state;
    enum LEXER_STATE return_state;
    bool parser_pause_flag;
    bool eof_emitted;
    bool frameset_ok;
    bool parser_mode_frozen;
} lexer;

lexer lexer_init(const char * filename, arena * perm); //TODO: get rid of this stupid nested initialization thing
token get_token(lexer * lexer);
string token_to_string(token t, arena * a);

#endif

