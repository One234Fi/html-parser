#include "common.h"
#include "input.h"
#include "tokenizer.h"
#include "mem.h"
#include "vector.h"

AttributeList * attribute_list_init();

token * token_init(TOKEN_TYPE type) {
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


