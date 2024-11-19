#ifndef TOKEN_MODULE_H
#define TOKEN_MODULE_H

#include "arena/arena.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct HTML_ATTRIBUTE {
    char* name;
    char* value;
} attribute;

typedef struct ATTRIBUTE_LIST {
    attribute * data;
    ptrdiff_t len;
    ptrdiff_t cap;
} AttributeList;

typedef enum TOKEN_TYPE {
    DOCTYPE,
    START_TAG,
    END_TAG,
    COMMENT,
    CHARACTER,
    END_OF_FILE,
    TOKEN_TYPE_COUNT,
    TOKEN_TAG, 
} token_type;


//TODO: temporarily put here for token definitions
typedef struct {
    char * data;
    ptrdiff_t len;
    ptrdiff_t cap;
} vec_char;

typedef struct  {
    vec_char * data; 
    bool exists;
} opt_str;

typedef struct {
    int32_t * data;
    bool exists;
} opt_int;

typedef struct {
    attribute * data;
    ptrdiff_t len;
    ptrdiff_t cap;
} vec_attr;
//TODO: temporarily put here for definitions


typedef struct token_struct token;
struct token_struct {
    token_type type;
};

typedef struct TOKEN_DOCTYPE {
    token token;
    opt_str name;
    opt_int public_id;
    opt_int system_id;
    bool force_quirks;
} token_doctype;

typedef struct token_tag {
    opt_str name;
    vec_attr attributes;
    bool self_closing;
} token_tag;

typedef struct TOKEN_START_TAG {
    token token;
    token_tag token_tag;
} token_start_tag;

typedef struct TOKEN_END_TAG {
    token token;
    token_tag token_tag;
} token_end_tag;

typedef struct TOKEN_COMMENT {
    token token;
    vec_char data;
} token_comment;

typedef struct TOKEN_CHARACTER {
    token token;
    char data;
} token_character;

typedef struct TOKEN_END_OF_FILE {
    token token;
} token_eof;


token * token_doctype_init(arena * a);
token * token_start_tag_init(arena * a);
token * token_end_tag_init(arena * a);
token * token_comment_init(arena * a);
token * token_character_init(arena * a, char c);
token * token_eof_init(arena * a);

void emit_token(token * token);

#endif
