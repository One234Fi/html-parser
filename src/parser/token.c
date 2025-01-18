#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "parser/token.h"
#include "common.h"

token token_init(token_type type) {
    token t = {0};
    t.type = type;
    return t;
}

token token_doctype_init() {
    return token_init(DOCTYPE);
}

token token_start_tag_init() {
    return token_init(START_TAG);
}

token token_end_tag_init() {
    return token_init(END_TAG);
}

token token_comment_init() {
    return token_init(COMMENT);
}

token token_character_init(char c) {
    token t = token_init(CHARACTER);
    t.character.data = c;
    return t;
}

token token_eof_init() {
    return token_init(END_OF_FILE);
}

char * tok_templs[] = {
    "DOCTYPE %s",
    "<%s>",
    "</%s>",
    "<!-- %s -->",
    "%c",
    "EOF"
};

char * token_to_string(token t) {
    switch (t.type) {
        case DOCTYPE: {
                size l = snprintf(NULL, 0, tok_templs[DOCTYPE], 
                        opt_get(&t.doctype.name, string)->data);
                char * buf = malloc(l + 1);
                sprintf(buf, tok_templs[DOCTYPE], 
                        opt_get(&t.doctype.name, string)->data);
                buf[l] = 0;
                return buf;
            }
            break;
        case START_TAG: {
                size l = snprintf(NULL, 0, tok_templs[START_TAG], 
                        opt_get(&t.start_tag.name, string)->data);
                char * buf = malloc(l + 1);
                buf[l] = 0;
                sprintf(buf, tok_templs[START_TAG], 
                        opt_get(&t.start_tag.name, string)->data);
                return buf;
            }
            break;

        case END_TAG: {
                size l = snprintf(NULL, 0, tok_templs[END_TAG], 
                        ((string *) t.end_tag.name.val)->data);
                char * buf = malloc(l + 1);
                buf[l] = 0;
                sprintf(buf, tok_templs[END_TAG], 
                        opt_get(&t.end_tag.name, string)->data);
                return buf;
            } 
            break;
        case COMMENT: {
                size l = snprintf(NULL, 0, tok_templs[COMMENT], 
                        t.comment.data.data);
                char * buf = malloc(l + 1);
                buf[l] = 0;
                sprintf(buf, tok_templs[COMMENT], t.comment.data.data);
                return buf;
            } 
            break;
        case CHARACTER: {
                size l = snprintf(NULL, 0, tok_templs[CHARACTER], 
                        t.character.data);
                char * buf = malloc(l + 1);
                buf[l] = 0;
                sprintf(buf, tok_templs[CHARACTER], t.character.data);
                return buf;
            }  
            break;
        case END_OF_FILE: {
                char * out = malloc(sizeof("EOF"));
                memcpy(out, "EOF", sizeof("EOF"));
                return out; 
            }
        default:
            return NULL;
    }

    return NULL;
}
