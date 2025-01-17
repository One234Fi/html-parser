#include "parser/token.h"
#include "tokenizer.h"
#include "node_stack.h"
#include "node_types.h"
#include "common.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>



void tree_construction_phase(token input);
void tree_construction_dispatcher(token input);


bool has_attribute(node n, char* attr_type);

bool has_attribute(node n, char *attr_type) {
    LOG_WARN("UNIMPLEMENTED");
    return false;
}

#define ATTRIBUTE_ENCODING "attribute_encoding_placeholder"

node get_current_node();
node get_adjusted_current_node();

bool in_html_namespace(node n) {
    LOG_WARN("UNIMPLEMENTED");
    return false;
}

bool is_html_integration_point(node n);

bool is_mathml_annotation_xml_element(node n);

bool is_mathml_annotation_xml_element(node n) {
    LOG_WARN("UNIMPLEMENTED");
    return false;
}

typedef struct tree_construction_state {
    node_stack* open_elements_stack;
    enum INSERTION_MODE_TYPE insertion_mode;
} tree_construction_state;

static tree_construction_state state;

void tree_construction_init() {
    state = (tree_construction_state) {
        .insertion_mode = INSERTION_MODE_INITIAL
    };
}

void tree_construction_phase(token input) {
    tree_construction_dispatcher(input);
}

void tree_construction_dispatcher(token input) {
//    node n = get_adjusted_current_node();
//    if ((node_stack_is_empty(state.open_elements_stack))
//        || (in_html_namespace(n)) 
//        || (is_mathml_text_integration_point(n)
//            && input.type == START_TAG
//            && (strncmp(input.start_tag.name.string.data, "mglyph", strlen("mglyph")) != 0)
//            && (strncmp(input.start_tag.name.string.data, "malignmark", strlen("malignmark")) != 0)) 
//        || (is_mathml_text_integration_point(n)
//            && input.type == CHARACTER) 
//        || (is_mathml_annotation_xml_element(n)
//            && input.type == START_TAG
//            && (strncmp(input.start_tag.name.string.data, "svg", strlen("svg")) == 0)) 
//        || (is_html_integration_point(n)
//            && input.type == START_TAG) 
//        || (is_html_integration_point(n)
//            && input.type == CHARACTER)
//        || (input.type == END_OF_FILE)) {
//
//        //insertion mode processing
//        //process();
//    } else {
//        //foreign content processing
//    }
}

node get_current_node() {
    return node_stack_peek(state.open_elements_stack);
}

node get_adjusted_current_node() { 
    //only return the current node because the
    //fragment parsing alg is not implemented 
    return node_stack_peek(state.open_elements_stack);
}

//TODO: put this somewhere else
bool compare_str_ignore_case(const char* a, const size_t n1, const char* b, const size_t n2) {
    char t1[n1] = {};
    for (size_t i = 0; i < n1; i++) {
        t1[i] = (char) tolower(a[i]);
    }

    char t2[n2] = {};
    for (size_t i = 0; i < n2; i++) {
        t2[i] = (char) tolower(b[i]);
    }

    size_t l = n2;
    if (n1 < n2) {
        l = n1;
    }
    return strncmp(a, b, l) == 0;
}


bool is_html_integration_point(node n) {
    if (n.type == MATHML_ANNOTATION_XML_ELEMENT 
            && has_attribute(n, ATTRIBUTE_ENCODING)
            && (compare_str_ignore_case(
                    n.name.data, 
                    n.name.len, 
                    "text/html", 
                    strlen("text/html"))
                || 
                compare_str_ignore_case(
                    n.name.data, 
                    n.name.len, 
                    "application/xhtml+xml", 
                    strlen("application/xhtml+xml")))) {
        return true;
    }

    return n.type == SVG_FOREIGN_OBJECT_ELEMENT
        || n.type == SVG_DESC_ELEMENT
        || n.type == SVG_TITLE_ELEMENT;
}

typedef enum insertion_mode {
    INITIAL,
    BEFORE_HTML,
    BEFORE_HEAD,
    IN_HEAD,
    IN_HEAD_NOSCRIPT,
    AFTER_HEAD,
    IN_BODY,
    TEXT,
    IN_TABLE,
    IN_TABLE_TEXT, 
    IN_CAPTION, 
    IN_COLUMN_GROUP,
    IN_TABLE_BODY,
    IN_ROW,
    IN_CELL,
    IN_SELECT,
    IN_SELECT_IN_TABLE,
    IN_TEMPLATE,
    AFTER_BODY,
    IN_FRAMESET,
    AFTER_FRAMESET,
    AFTER_AFTER_BODY,
    AFTER_AFTER_FRAMESET
} insertion_mode;

void process(parser * p, token t) {
    switch (p->insertion_mode) {
        case INSERTION_MODE_INITIAL: 
            {
                switch (t.type) {
                    case CHARACTER: 
                        {
                            if (t.character.data == '\t' ||
                                t.character.data == '\n' ||
                                t.character.data == '\f' ||
                                t.character.data == '\r' ||
                                t.character.data == ' ') {
                                //ignore
                                break;
                            }
                            //if not an iframe srcdoc doc, then parse error
                            p->insertion_mode = INSERTION_MODE_BEFORE_HTML;
                            process(p, t); //reprocess
                        }
                        break;
                    case COMMENT:
                        {
                            //TODO: append a comment to doc node
                        }
                        break;
                    case DOCTYPE:
                        {
                            if (!t.doctype.name.exists ||
//                                    !string_equal(t.doctype.name.string, make_string("html")) ||
                                    t.doctype.public_id.exists) {
                                //parse_error
                            }
                            //append a doctype node to doc node
                            //  name: t.name : ""
                            //  public_id: t.public_id : ""
                            //  system_id: t.system_id: ""
                            //
                            //  then handle quicks mode check
                            p->insertion_mode = INSERTION_MODE_BEFORE_HTML;
                            break;
                        }
                        break;
                    default:
                        //if not an iframe srcdoc doc, then parse error
                        p->insertion_mode = INSERTION_MODE_BEFORE_HTML;
                        process(p, t); //reprocess
                }
            }
            break;
        case BEFORE_HTML: break;
        case BEFORE_HEAD: break;
        case IN_HEAD: break;
        case IN_HEAD_NOSCRIPT: break;
        case AFTER_HEAD: break;
        case IN_BODY: break;
        case TEXT: break;
        case IN_TABLE: break;
        case IN_TABLE_TEXT: break; 
        case IN_CAPTION: break; 
        case IN_COLUMN_GROUP: break;
        case IN_TABLE_BODY: break;
        case IN_ROW: break;
        case IN_CELL: break;
        case IN_SELECT: break;
        case IN_SELECT_IN_TABLE: break;
        case IN_TEMPLATE: break;
        case AFTER_BODY: break;
        case IN_FRAMESET: break;
        case AFTER_FRAMESET: break;
        case AFTER_AFTER_BODY: break;
        case AFTER_AFTER_FRAMESET: break;
        default:
            LOG_ERROR("Invalid insertion mode type!");
    }
}
