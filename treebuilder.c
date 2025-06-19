#include "token.h"
#include "tokenizer.h"
#include "common.h"
#include "opt.h"
#include "str.h"
#include "types.h"
#include <stdbool.h>


typedef enum NODE_TYPES {
    MATHML_MI_ELEMENT,
    MATHML_MO_ELEMENT,
    MATHML_MN_ELEMENT,
    MATHML_MS_ELEMENT,
    MATHML_MTEXT_ELEMENT,
    MATHML_ANNOTATION_XML_ELEMENT,
    SVG_FOREIGN_OBJECT_ELEMENT,
    SVG_DESC_ELEMENT,
    SVG_TITLE_ELEMENT,
} NODE_TYPES;

#define is_mathml_text_integration_point(node) \
        (node.type == MATHML_MI_ELEMENT \
        || node.type == MATHML_MO_ELEMENT \
        || node.type == MATHML_MN_ELEMENT \
        || node.type == MATHML_MS_ELEMENT \
        || node.type == MATHML_MTEXT_ELEMENT)

typedef struct node node;
typedef struct nodes nodes;

struct nodes {
    node * data;
    size len;
    size cap;
};

struct node {
    NODE_TYPES type;
    string name;
    node * parent;
    nodes children;
};


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
    nodes open_elements_stack;
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
    node n = get_adjusted_current_node();
    string start_tag_name = *opt_unwrap(&input.start_tag.name, string, &String(""));
    if (state.open_elements_stack.len <= 0
        || (in_html_namespace(n))
        || (is_mathml_text_integration_point(n)
            && input.type == START_TAG
            && (!s_equal(start_tag_name, String("mglyph")))
            && (!s_equal(start_tag_name, String("malignmark"))))
        || (is_mathml_text_integration_point(n)
            && input.type == CHARACTER)
        || (is_mathml_annotation_xml_element(n)
            && input.type == START_TAG
            && (s_equal(start_tag_name, String("svg"))))
        || (is_html_integration_point(n)
            && input.type == START_TAG) 
        || (is_html_integration_point(n)
            && input.type == CHARACTER)
        || (input.type == END_OF_FILE)) {

        //insertion mode processing
        //process();
    } else {
        //foreign content processing
    }
}

node get_current_node() {
    return state.open_elements_stack.data[state.open_elements_stack.len-1];
}

node get_adjusted_current_node() { 
    //only return the current node because the
    //fragment parsing alg is not implemented 
    return get_current_node();
}



bool is_html_integration_point(node n) {
    if (n.type == MATHML_ANNOTATION_XML_ELEMENT 
            && has_attribute(n, ATTRIBUTE_ENCODING)
            && (s_equal_ignore_case(n.name, String("text/html")) || 
                s_equal_ignore_case(n.name, String("application/xhtml+xml")))) {
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


void insertion_mode_initial(parser * p, token t); 
void before_html(parser * p, token t);
void before_head(parser * p, token t);
void in_head(parser * p, token t);
void in_head_noscript(parser * p, token t);
void after_head(parser * p, token t);
void in_body(parser * p, token t);
void text(parser * p, token t);
void in_table(parser * p, token t);
void in_table_text(parser * p, token t); 
void in_caption(parser * p, token t); 
void in_column_group(parser * p, token t);
void in_table_body(parser * p, token t);
void in_row(parser * p, token t);
void in_cell(parser * p, token t);
void in_select(parser * p, token t);
void in_select_in_table(parser * p, token t);
void in_template(parser * p, token t);
void after_body(parser * p, token t);
void in_frameset(parser * p, token t);
void after_frameset(parser * p, token t);
void after_after_body(parser * p, token t);
void after_after_frameset(parser * p, token t);

void process(parser * p, token t) {
    switch (p->insert_mode) {
        case INSERTION_MODE_INITIAL: insertion_mode_initial(p, t); break;
        case BEFORE_HTML: before_html(p, t); break;
        case BEFORE_HEAD: before_head(p, t); break;
        case IN_HEAD: in_head(p, t); break;
        case IN_HEAD_NOSCRIPT: in_head_noscript(p, t); break;
        case AFTER_HEAD: after_head(p, t); break;
        case IN_BODY: in_body(p, t); break;
        case TEXT: text(p,t); break;
        case IN_TABLE: in_table(p, t); break;
        case IN_TABLE_TEXT: in_table_text(p, t); break; 
        case IN_CAPTION: in_caption(p, t); break; 
        case IN_COLUMN_GROUP: in_column_group(p, t); break;
        case IN_TABLE_BODY: in_table_body(p, t); break;
        case IN_ROW: in_row(p, t); break;
        case IN_CELL: in_cell(p, t); break;
        case IN_SELECT: in_select(p, t); break;
        case IN_SELECT_IN_TABLE: in_select_in_table(p, t); break;
        case IN_TEMPLATE: in_template(p, t); break;
        case AFTER_BODY: after_body(p, t); break;
        case IN_FRAMESET: in_frameset(p, t); break;
        case AFTER_FRAMESET: after_frameset(p, t); break;
        case AFTER_AFTER_BODY: after_after_body(p, t); break;
        case AFTER_AFTER_FRAMESET: after_after_frameset(p, t); break;
        default:
            LOG_ERROR("Invalid insertion mode type!");
    }
}

void insertion_mode_initial(parser * p, token t) {
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
                p->insert_mode = INSERTION_MODE_BEFORE_HTML;
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
                        !s_equal(*opt_unwrap(&t.doctype.name, string, &String("")), String("html")) ||
                        t.doctype.public_id.exists) {
                    //parse_error
                }
                //append a doctype node to doc node
                //  name: t.name : ""
                //  public_id: t.public_id : ""
                //  system_id: t.system_id: ""
                //
                //  then handle quircks mode check
                p->insert_mode = INSERTION_MODE_BEFORE_HTML;
                break;
            }
            break;
        default:
            //if not an iframe srcdoc doc, then parse error
            p->insert_mode = INSERTION_MODE_BEFORE_HTML;
            process(p, t); //reprocess
    }
}

void before_html(parser * p, token t) {
    switch (t.type) {
        case DOCTYPE: 
            {
                //Parse error, ignore
            }
            break;
        case COMMENT:
            {
                //insert a comment
            }
            break;
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
            }
        case START_TAG:
            {
            }
    }
}

void before_head(parser * p, token t) {}
void in_head(parser * p, token t) {}
void in_head_noscript(parser * p, token t) {}
void after_head(parser * p, token t) {}
void in_body(parser * p, token t) {}
void text(parser * p, token t) {}
void in_table(parser * p, token t) {}
void in_table_text(parser * p, token t) {} 
void in_caption(parser * p, token t) {} 
void in_column_group(parser * p, token t) {}
void in_table_body(parser * p, token t) {}
void in_row(parser * p, token t) {}
void in_cell(parser * p, token t) {}
void in_select(parser * p, token t) {}
void in_select_in_table(parser * p, token t) {}
void in_template(parser * p, token t) {}
void after_body(parser * p, token t) {}
void in_frameset(parser * p, token t) {}
void after_frameset(parser * p, token t) {}
void after_after_body(parser * p, token t) {}
void after_after_frameset(parser * p, token t) {}
