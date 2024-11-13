#include "common.h"
#include "input.h"
#include "mem.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>

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


//TODO: temporarily put here for token definitions
typedef struct {
    char * data;
    ptrdiff_t len;
    ptrdiff_t cap;
} vec_char;

typedef struct  {
    char * data; 
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

struct TOKEN_DOCTYPE {
    opt_str name;
    opt_int public_id;
    opt_int system_id;
    bool force_quirks;
};

struct TOKEN_START_TAG {
    opt_str tag_name;
    bool self_closing;
    vec_attr attributes;
};

struct TOKEN_END_TAG {
    opt_str tag_name;
    bool self_closing;
    vector* attributes;
};

struct TOKEN_COMMENT {
    vec_char data;
};

struct TOKEN_CHARACTER {
    vec_char data;
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


AttributeList * attribute_list_init();
void attribute_list_destroy(AttributeList * list);

token * 
token_init(const TOKEN_TYPE type) {
    token * t = MALLOC(sizeof(token));
    t->type = type;
    switch (type) {
        case DOCTYPE:
            t->val.doctype.name = opt_init(sizeof(string_buffer));
            t->val.doctype.public_id = opt_init(sizeof(int));
            t->val.doctype.system_id = opt_init(sizeof(int));
            t->val.doctype.force_quirks = false;
            break;
        case START_TAG:
            t->val.start_tag.tag_name = opt_init(sizeof(string_buffer));
            t->val.start_tag.self_closing = false; 
            t->val.start_tag.attributes = vector_init(sizeof(attribute)); //TODO: attribute type
            break;
        case END_TAG:
            t->val.end_tag.tag_name = opt_init(sizeof(string_buffer));
            t->val.end_tag.self_closing = false; 
            t->val.end_tag.attributes = vector_init(sizeof(attribute)); //TODO: attribute type
            break;
        case COMMENT:
            t->val.comment.data = NULL; //TODO: vec type
            break;
        case CHARACTER:
            t->val.character.data = NULL; //This might also need to be a vec?...
            break;
        case END_OF_FILE:
            break;
        default:
            LOG_ERROR("Can't create nonexistant token");
    }
    
    return t;
}

void
token_destroy(token * t) {
    switch (t->type) {
        case DOCTYPE:
            string_buffer_destroy(t->val.doctype.name->data);
            opt_destroy(t->val.doctype.name);
            opt_destroy(t->val.doctype.public_id);
            opt_destroy(t->val.doctype.system_id);
            break;
        case START_TAG:
            vector_destroy(t->val.start_tag.attributes);
            break;
        case END_TAG:
            vector_destroy(t->val.end_tag.attributes);
            break;
        case COMMENT:
            //vector_destroy(t->val.comment.data);
            break;
        case CHARACTER:
            break;
        case END_OF_FILE:
            break;
        default:
            LOG_ERROR("Can't destroy nonexistant token type");
    }
    FREE(t);
}


