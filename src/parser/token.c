#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "parser/token.h"
#include "types/vector.h"
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

string token_to_string(token t, arena * a) {
    switch (t.type) {
        case DOCTYPE: {
                string ret = make_string("DOCTYPE \"");
                ret = s_cat(ret, *opt_get(&t.doctype.name, string), a);
                ret = s_cat(ret, make_string("\""), a);
                return ret;
            }
            break;

        case START_TAG: {
                string ret = make_string("<");
                ret = s_cat(ret, *opt_get(&t.start_tag.name, string), a);
                ret = s_cat(ret, make_string(">"), a);
                return ret;
            }
            break;

        case END_TAG: {
                string ret = make_string("</");
                ret = s_cat(ret, *opt_get(&t.end_tag.name, string), a);
                ret = s_cat(ret, make_string(">"), a);
                return ret;
            } 
            break;

        case COMMENT: {
                string ret = make_string("<!-- ");
                ret = s_cat(ret, t.comment.data, a);
                ret = s_cat(ret, make_string(" -->"), a);
                return ret;
            } 
            break;

        case CHARACTER: {
                string ret = {0};
                *push(&ret, a) = t.character.data;
                return ret;
            }  
            break;

        case END_OF_FILE: {
                return s_clone(make_string("EOF\n"), a); 
            }

        default:
            return make_string("");
    }

    return make_string("");
}
