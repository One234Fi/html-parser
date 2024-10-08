#include "tokenizer.h"
#include "node_stack.h"
#include "node_types.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>



void tree_construction_phase(struct TOKEN_STRUCT input);
void tree_construction_dispatcher(struct TOKEN_STRUCT input);


bool has_attribute(node n, char* attr_type);
#define ATTRIBUTE_ENCODING "attribute_encoding_placeholder"

node get_current_node();
node get_adjusted_current_node();

bool in_html_namespace(node n);
bool is_html_integration_point(node n);
bool is_mathml_annotation_xml_element(node n);

typedef struct tree_construction_state {
    node_stack* open_elements_stack;
} tree_construction_state;

static tree_construction_state state;


void tree_construction_phase(TOKEN_STRUCT input) {
    tree_construction_dispatcher(input);
}

void tree_construction_dispatcher(struct TOKEN_STRUCT input) {
    node n = get_adjusted_current_node();
    if ((node_stack_is_empty(state.open_elements_stack))
        || (in_html_namespace(n)) 
        || (is_mathml_text_integration_point(n)
            && input.type == START_TAG
            && (strncmp(input.val.start_tag.tag_name, "mglyph", strlen("mglyph")) != 0)
            && (strncmp(input.val.start_tag.tag_name, "malignmark", strlen("malignmark")) != 0)) 
        || (is_mathml_text_integration_point(n)
            && input.type == CHARACTER) 
        || (is_mathml_annotation_xml_element(n)
            && input.type == START_TAG
            && (strncmp(input.val.start_tag.tag_name, "svg", strlen("svg")) == 0)) 
        || (is_html_integration_point(n)
            && input.type == START_TAG) 
        || (is_html_integration_point(n)
            && input.type == CHARACTER)
        || (input.type == END_OF_FILE)) {

        //insertion mode processing
    } else {
        //foreign content processing
    }
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
                    n.name.length, 
                    "text/html", 
                    strlen("text/html"))
                || 
                compare_str_ignore_case(
                    n.name.data, 
                    n.name.length, 
                    "application/xhtml+xml", 
                    strlen("application/xhtml+xml")))) {
        return true;
    }

    return n.type == SVG_FOREIGN_OBJECT_ELEMENT
        || n.type == SVG_DESC_ELEMENT
        || n.type == SVG_TITLE_ELEMENT;
}
