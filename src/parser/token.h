#ifndef TOKEN_MODULE_H
#define TOKEN_MODULE_H

#include "types/opt.h"
#include "types/str.h"
#include "types/types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    string name;
    string value;
} attr;

typedef struct {
    attr * data;
    size len;
    size cap;
} attrs;

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

typedef struct {
    opt_str name;
    opt_i32 public_id;
    opt_i32 system_id;
    bool force_quirks;
} t_doctype;

typedef struct {
    opt_str name;
    attrs attributes;
    bool self_closing;
} t_start_tag;

typedef struct {
    opt_str name;
    attrs attributes;
    bool self_closing;
} t_end_tag;

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
        t_start_tag start_tag;
        t_end_tag end_tag;
        t_comment comment;
        t_character character;
        t_eof eof;
    };
    token_type type;
} token;

token token_doctype_init();
token token_start_tag_init();
token token_end_tag_init();
token token_comment_init();
token token_character_init(char c);
token token_eof_init();
char * token_to_string(token t);

bool emit_token(token t);

#endif
