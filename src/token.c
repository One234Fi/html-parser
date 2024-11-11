#include "common.h"
#include "input.h"
#include "tokenizer.h"
#include "mem.h"
#include "vector.h"

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


