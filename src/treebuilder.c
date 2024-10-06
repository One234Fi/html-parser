#include "tokenizer.h"
#include "node_stack.h"
#include <cstdio>
#include <string.h>


void tree_construction_phase(struct TOKEN_STRUCT input);
void tree_construction_dispatcher(struct TOKEN_STRUCT input);



node get_current_node();
node get_adjusted_current_node();

bool in_html_namespace(node n);
bool is_html_integration_point(node n);
bool is_mathml_text_integration_point(node n);
bool is_mathml_annotation_xml_element(node n);

typedef struct tree_construction_state {
    node_stack* open_elements_stack;
} tree_construction_state;

static tree_construction_state state;


void tree_construction_phase(TOKEN_STRUCT input) {
    tree_construction_dispatcher(input);
}

void tree_construction_dispatcher(struct TOKEN_STRUCT input) {
    if ((node_stack_is_empty(state.open_elements_stack))
        || (in_html_namespace(get_adjusted_current_node())) 
        || (is_mathml_text_integration_point(get_adjusted_current_node())
            && input.type == START_TAG
            && (strncmp(input.val.start_tag.tag_name, "mglyph", strlen("mglyph")) != 0)
            && (strncmp(input.val.start_tag.tag_name, "malignmark", strlen("malignmark")) != 0)) 
        || (is_mathml_text_integration_point(get_adjusted_current_node())
            && input.type == CHARACTER) 
        || (is_mathml_annotation_xml_element(get_adjusted_current_node())
            && input.type == START_TAG
            && (strncmp(input.val.start_tag.tag_name, "svg", strlen("svg")) == 0)) 
        || (is_html_integration_point(get_adjusted_current_node())
            && input.type == START_TAG) 
        || (is_html_integration_point(get_adjusted_current_node())
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
