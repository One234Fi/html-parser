#include "common.h"
#include "input.h"
#include "mem.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>
#include "optional/optional.h"

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
    TOKEN_TYPE_COUNT
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
typedef struct token_vtable {
    void * (*set_identifier)(token * t, char * val, size_t len);
    void * (*emit)(token * t);
} token_vtable;
struct token_struct {
    token_vtable * vtable;
    token_type type;
};


typedef struct token_doctype_vtable {
    void * (*set_force_quirks)(token * t, bool b);
} token_doctype_vtable;
typedef struct TOKEN_DOCTYPE {
    token token;
    opt_str name;
    opt_int public_id;
    opt_int system_id;
    bool force_quirks;
    token_doctype_vtable * vtable;
} token_doctype;


typedef struct token_tag_vtable {
    void * (*new_attribute)(token * t);
    void * (*push_attribute_name)(token * t, char c);
    void * (*push_attribute_value)(token * t, char c);
    void * (*set_self_closing)(token * t, bool b);
} token_tag_vtable;
typedef struct TOKEN_START_TAG {
    token token;
    opt_str name;
    vec_attr attributes;
    bool self_closing;
} token_start_tag;

typedef struct TOKEN_END_TAG {
    token token;
    opt_str name;
    vec_attr attributes;
    bool self_closing;
} token_end_tag;


typedef struct token_comment_vtable {
    void * (*push_data)(token * t, char c);
} token_comment_vtable;
typedef struct TOKEN_COMMENT {
    token token;
    vec_char data;
} token_comment;


typedef struct token_character_vtable {
    void * (*set_data)(token * t, char c);
} token_character_vtable;
typedef struct TOKEN_CHARACTER {
    token token;
    char data;
} token_character;

typedef struct TOKEN_END_OF_FILE {
    token token;
} token_eof;


token * token_doctype_init();
token * token_start_tag_init();
token * token_end_tag_init();
token * token_comment_init();
token * token_character_init();
token * token_eof_init();


void emit_token(token * token);


token *
token_init(const token_type type, arena * a) {
    token * t = new(a, token);
    t->type = type;
    switch (type) {
        case DOCTYPE:
            opt_init(t->val.doctype.name, vec_char, a);
            opt_init(t->val.doctype.public_id, int32_t, a);
            opt_init(t->val.doctype.system_id, int32_t, a);
            t->val.doctype.force_quirks = false;
            break;
        case START_TAG:
            opt_init(t->val.start_tag.tag_name, vec_char, a);
            t->val.start_tag.self_closing = false; 
            break;
        case END_TAG:
            opt_init(t->val.end_tag.tag_name, vec_char, a);
            t->val.end_tag.self_closing = false; 
            break;
        case COMMENT:
            //vec does not need to be initialized 
            break;
        case CHARACTER:
            //vec does not need to be initialized 
            break;
        case END_OF_FILE:
            break;
        default:
            LOG_ERROR("Can't create nonexistant token");
    }
    
    return t;
}

