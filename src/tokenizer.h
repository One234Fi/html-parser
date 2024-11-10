#ifndef HTML_TOKENIZER_MODULE
#define HTML_TOKENIZER_MODULE

/*
 * Types used by the tokenizer
 * 08/24/2024
 */

#include "input.h"
#include "option.h"
#include "vector.h"
#include <stdbool.h>
#include <stdlib.h>


void execute();

typedef struct HTML_ATTRIBUTE {
    char* name;
    char* value;
} attribute;

typedef struct ATTRIBUTE_LIST {
    attribute * data;
    size_t length;
    size_t max;
} AttributeList;

typedef enum TOKEN_TYPE {
    DOCTYPE,
    START_TAG,
    END_TAG,
    COMMENT,
    CHARACTER,
    END_OF_FILE,
    TOKEN_TYPE_COUNT
} TOKEN_TYPE;

//TODO: create generic optional type for "missing" fields
struct TOKEN_DOCTYPE {
    opt * name;                 // string
    opt * public_id;            // int
    opt * system_id;            // int
    bool force_quirks;  
};

struct TOKEN_START_TAG {
    opt * tag_name;             // string
    bool self_closing;
    vector* attributes;         // attribute
};

struct TOKEN_END_TAG {
    opt * tag_name;             // string
    bool self_closing;
    vector* attributes;         // attribute
};

struct TOKEN_COMMENT {
    char* data;
};

struct TOKEN_CHARACTER {
    char* data;
};

struct TOKEN_END_OF_FILE {
};

union TOKEN_UNION {
    struct TOKEN_DOCTYPE doctype;
    struct TOKEN_START_TAG start_tag;
    struct TOKEN_END_TAG end_tag;
    struct TOKEN_COMMENT comment;
    struct TOKEN_CHARACTER character;
    struct TOKEN_END_OF_FILE eof;
};

typedef struct TOKEN_STRUCT {
    union TOKEN_UNION val;
    enum TOKEN_TYPE type;
} token;

enum TOKENIZER_STATE_TYPE {
    DATA_STATE,
    RCDATA_STATE,
    RAWTEXT_STATE,
    SCRIPT_DATA_STATE,
    PLAINTEXT_STATE,
    TAG_OPEN_STATE,
    END_TAG_OPEN_STATE,
    TAG_NAME_STATE,
    RCDATA_LESS_THAN_SIGN_STATE,
    RCDATA_END_TAG_OPEN_STATE,
    RCDATA_END_TAG_NAME_STATE,
    RAWTEXT_LESS_THAN_SIGN_STATE,
    RAWTEXT_END_TAG_OPEN_STATE,
    RAWTEXT_END_TAG_NAME_STATE,
    SCRIPT_DATA_LESS_THAN_SIGN_STATE,
    SCRIPT_DATA_END_TAG_OPEN_STATE,
    SCRIPT_DATA_END_TAG_NAME_STATE,
    SCRIPT_DATA_ESCAPE_START_STATE,
    SCRIPT_DATA_ESCAPE_START_DASH_STATE,
    SCRIPT_DATA_ESCAPED_STATE,
    SCRIPT_DATA_ESCAPED_DASH_STATE,
    SCRIPT_DATA_ESCAPED_DASH_DASH_STATE,
    SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE,
    SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE,
    SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE,
    SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE,
    SCRIPT_DATA_DOUBLE_ESCAPED_STATE,
    SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE,
    SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE,
    SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE,
    SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE,
    BEFORE_ATTRIBUTE_NAME_STATE,
    ATTRIBUTE_NAME_STATE,
    AFTER_ATTRIBUTE_NAME_STATE,
    BEFORE_ATTRIBUTE_VALUE_STATE,
    ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE,
    ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE,
    ATTRIBUTE_VALUE_UNQUOTED_STATE,
    AFTER_ATTRIBUTE_VALUE_QUOTED_STATE,
    SELF_CLOSING_START_TAG_STATE,
    BOGUS_COMMENT_STATE,
    MARKUP_DECLARATION_OPEN_STATE,
    COMMENT_START_STATE,
    COMMENT_START_DASH_STATE,
    COMMENT_STATE,
    COMMENT_LESS_THAN_SIGN_STATE,
    COMMENT_LESS_THAN_SIGN_BANG_STATE,
    COMMENT_LESS_THAN_SIGN_BANG_DASH_STATE,
    COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH_STATE,
    COMMENT_END_DASH_STATE,
    COMMENT_END_STATE,
    COMMENT_END_BANG_STATE,
    DOCTYPE_STATE,
    BEFORE_DOCTYPE_NAME_STATE,
    DOCTYPE_NAME_STATE,
    AFTER_DOCTYPE_NAME_STATE,
    AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE,
    BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE,
    DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE,
    DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE,
    AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE,
    BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE,
    AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE,
    BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE,
    DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE,
    DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE,
    AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE,
    BOGUS_DOCTYPE_STATE,
    CDATA_SECTION_STATE,
    CDATA_SECTION_BRACKET_STATE,
    CDATA_SECTION_END_STATE,
    CHARACTER_REFERENCE_STATE,
    NAMED_CHARACTER_REFERENCE_STATE,
    AMBIGUOUS_AMPERSAND_STATE,
    NUMERIC_CHARACTER_REFERENCE_STATE,
    HEXADECIMAL_CHARACTER_REFERENCE_START_STATE,
    DECIMAL_CHARACTER_REFERENCE_START_STATE,
    HEXADECIMAL_CHARACTER_REFERENCE_STATE,
    DECIMAL_CHARACTER_REFERENCE_STATE,
    NUMERIC_CHARACTER_REFERENCE_END_STATE,
};

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

#endif
