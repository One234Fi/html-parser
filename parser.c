#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "fickit.h"

#include "lexer.h"
#include "parser.h"

enum HTML_PARSER_ERROR {
    //SPEC: Parse errors
    ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR,
    ABRUPT_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR,
    ABRUPT_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR,
    ABSENCE_OF_DIGITS_IN_NUMERIC_CHARACTER_REFERENCE_PARSE_ERROR,
    CDATA_IN_HTML_CONTENT_PARSE_ERROR,
    CHARACTER_REFERENCE_OUTSIDE_OF_UNICODE_RANGE_PARSE_ERROR,
    CONTROL_CHARACTER_IN_INPUT_STREAM_PARSE_ERROR,
    CONTROL_CHARACTER_REFERENCE_PARSE_ERROR,
    DUPLICATE_ATTRIBUTE_PARSE_ERROR,
    END_TAG_WITH_ATTRIBUTES_PARSE_ERROR,
    END_TAG_WITH_TRAILING_SOLIDUS_PARSE_ERROR,
    EOF_BEFORE_TAG_NAME_PARSE_ERROR,
    EOF_IN_CDATA_PARSE_ERROR,
    EOF_IN_COMMENT_PARSE_ERROR,
    EOF_IN_DOCTYPE_PARSE_ERROR,
    EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR,
    EOF_IN_TAG_PARSE_ERROR,
    INCORRECTLY_CLOSED_COMMENT_PARSE_ERROR,
    INCORRECTLY_OPENED_COMMENT_PARSE_ERROR,
    INVALID_CHARACTER_SEQUENCE_AFTER_DOCTYPE_NAME_PARSE_ERROR,
    INVALID_FIRST_CHARACTER_OF_TAG_NAME_PARSE_ERROR,
    MISSING_ATTRIBUTE_VALUE_PARSE_ERROR,
    MISSING_DOCTYPE_NAME_PARSE_ERROR,
    MISSING_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR,
    MISSING_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR,
    MISSING_END_TAG_NAME_PARSE_ERROR,
    MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR,
    MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR,
    MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE_PARSE_ERROR,
    MISSING_WHITESPACE_AFTER_DOCTYPE_PUBLIC_KEYWORD_PARSE_ERROR,
    MISSING_WHITESPACE_AFTER_DOCTYPE_SYSTEM_KEYWORD_PARSE_ERROR,
    MISSING_WHITESPACE_BEFORE_DOCTYPE_NAME_PARSE_ERROR,
    MISSING_WHITESPACE_BETWEEN_ATTRIBUTES_PARSE_ERROR,
    MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_PARSE_ERROR,
    NESTED_COMMENT_PARSE_ERROR,
    NONCHARACTER_CHARACTER_REFERENCE_PARSE_ERROR,
    NONCHARACTER_IN_INPUT_STREAM_PARSE_ERROR,
    NON_VOID_HTML_ELEMENT_START_TAG_WITH_TRAILING_SOLIDUS_PARSE_ERROR,
    NULL_CHARACTER_REFERENCE_PARSE_ERROR,
    SURROGATE_CHARACTER_REFERENCE_PARSE_ERROR,
    SURROGATE_IN_INPUT_STREAM_PARSE_ERROR,
    UNEXPECTED_CHARACTER_AFTER_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR,
    UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR,
    UNEXPECTED_CHARACTER_IN_UNQUOTED_ATTRIBUTE_VALUE_PARSE_ERROR,
    UNEXPECTED_EQUALS_SIGN_BEFORE_ATTRIBUTE_NAME_PARSE_ERROR,
    UNEXPECTED_NULL_CHARACTER_PARSE_ERROR,
    UNEXPECTED_QUESTION_MARK_INSTEAD_OF_TAG_NAME_PARSE_ERROR,
    UNEXPECTED_SOLIDUS_IN_TAG_PARSE_ERROR,
    UNKNOWN_NAMED_CHARACTER_REFERENCE_PARSE_ERROR,

    //NON-SPEC: internal errors
    INVALID_TOKENIZER_STATE_ERROR,
};

typedef enum node_family {
    MATHML_MI_ELEMENT,
    MATHML_MO_ELEMENT,
    MATHML_MN_ELEMENT,
    MATHML_MS_ELEMENT,
    MATHML_MTEXT_ELEMENT,
    MATHML_ANNOTATION_XML_ELEMENT,
    SVG_FOREIGN_OBJECT_ELEMENT,
    SVG_DESC_ELEMENT,
    SVG_TITLE_ELEMENT,
} node_family;

#define is_mathml_text_integration_point(node) \
        (node->type == MATHML_MI_ELEMENT \
        || node->type == MATHML_MO_ELEMENT \
        || node->type == MATHML_MN_ELEMENT \
        || node->type == MATHML_MS_ELEMENT \
        || node->type == MATHML_MTEXT_ELEMENT)


size stack_has_c(nodes stack, char * name) {
    for (size i = 0; i < stack.len; i++) {
        if (s_equal_c(stack.data[i].name, name)) {
            return i;
        }
    }
    return -1;
}

size stack_has(nodes stack, string name) {
    for (size i = 0; i < stack.len; i++) {
        if (s_equal(stack.data[i].name, name)) {
            return i;
        }
    }
    return -1;
}

node stack_pop(nodes stack) {
    return stack.data[--stack.len];
}

parser parser_init(const char* filename, arena * a) {
    parser p = {0};
    p.arena = a;
    p.frameset_ok = true;
    p.document.type = HTML_DOCUMENT;
    p.lex = lexer_init(filename, a);
    return p;
}


//TREEBUILDER

void tree_construction_phase(parser * p, token input);
void tree_construction_dispatcher(parser * p, token input);


bool has_attribute(node n, char* attr_type);

bool has_attribute(node n, char *attr_type) {
    LOG_WARN("UNIMPLEMENTED");
    return false;
}

#define ATTRIBUTE_ENCODING "attribute_encoding_placeholder"

node * get_current_node(parser * p);
node * get_adjusted_current_node(parser * p);

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

void tree_construction_phase(parser * p, token input) {
    tree_construction_dispatcher(p, input); //TODO: why did I do this??
}

void process(parser * p, token t); //FIXME
void tree_construction_dispatcher(parser * p, token input) {
    node * n = get_adjusted_current_node(p);
    if (p->open_elem_stack.len <= 0
        || (in_html_namespace(*n))
        || (is_mathml_text_integration_point(n)
            && input.type == START_TAG
            && (!s_equal_c(input.tag.name, "mglyph"))
            && (!s_equal_c(input.tag.name, "malignmark")))
        || (is_mathml_text_integration_point(n)
            && input.type == CHARACTER)
        || (is_mathml_annotation_xml_element(*n)
            && input.type == START_TAG
            && (s_equal_c(input.tag.name, "svg")))
        || (is_html_integration_point(*n)
            && input.type == START_TAG) 
        || (is_html_integration_point(*n)
            && input.type == CHARACTER)
        || (input.type == END_OF_FILE)) {

        //insertion mode processing
        process(p, input);
    } else {
        //foreign content processing
    }
}

node * get_current_node(parser * p) {
    return &p->open_elem_stack.data[p->open_elem_stack.len-1];
}

node * get_adjusted_current_node(parser * p) { 
    //TODO:
    //  only return the current node because the
    //  fragment parsing alg is not implemented 
    return get_current_node(p);
}


bool is_html_integration_point(node n) {
    //TODO: this is wrong/pseudocode...
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

static void (* parser_state_handlers[])(parser *, token) = {
    insertion_mode_initial,
    before_html,
    before_head,
    in_head,
    in_head_noscript,
    after_head,
    in_body,
    text,
    in_table,
    in_table_text,
    in_caption,
    in_column_group,
    in_table_body,
    in_row,
    in_cell,
    in_select,
    in_select_in_table,
    in_template,
    after_body,
    in_frameset,
    after_frameset,
    after_after_body,
    after_after_frameset,
};

void process(parser * p, token t) {
    parser_state_handlers[p->insert_mode](p, t);
}

node * push_child(node * parent, arena * a) {
    node * new_node = push(&parent->children, a);
    new_node->parent = parent;
    if (parent->children.len > 1) {
        new_node->prev = &parent->children.data[parent->children.len - 2];
    }
    return new_node;
}

void insert_comment(string data, node * where, arena * a) {
    node node = {0};
    node.type = HTML_COMMENT;
    node.comment.data = data;
    *push_child(where, a) = node;
}

node * get_adjusted_insert_location(parser * p, node * where, bool foster_parenting) {
    node * target = where == NULL ? get_current_node(p) : where;
    if (foster_parenting 
            && (target->type == HTML_TABLE
             || target->type == HTML_TBODY
             || target->type == HTML_TFOOT
             || target->type == HTML_THEAD
             || target->type == HTML_TR)) {
        //TODO: this is a dumb lazy way of doing this
        node * last_template = NULL;
        size last_template_index = -1;
        node * last_table = NULL;
        size last_table_index = -1;
        for (size i = 0; i < p->open_elem_stack.len; i++) {
            if (p->open_elem_stack.data[i].type == HTML_TEMPLATE) {
                last_template = p->open_elem_stack.data + i;
                last_template_index = i;
            } else if (p->open_elem_stack.data[i].type == HTML_TABLE) {
                last_table = p->open_elem_stack.data + i;
                last_table_index = i;
            }
        }

        bool template_lower = last_template_index > last_table_index;
        if (last_template && (!last_table || template_lower)) {
            target = target->template.contents;
            goto short_circuit;
        }

        if (!last_table) {
            target = &p->open_elem_stack.data[0]; //html element
            goto short_circuit;
        }

        if (last_table->parent) {
            target = last_table->parent;
            //TODO: target should be inserted before last_table
            goto short_circuit;
        }

        target = &p->open_elem_stack.data[last_table_index - 1];
    }

short_circuit:
    if (target->type == HTML_TEMPLATE) {
        return push_child(target->template.contents, p->arena);
    }

    return push_child(target, p->arena);
}

void insert_character_alg(parser * p, string data, node * where) {
    where = get_adjusted_insert_location(p, where, false);
    if (where->type == HTML_DOCUMENT) {
        return;
    }

    if (where->prev->type == HTML_TEXT) {
        string s = s_cat(where->prev->text.data, data, p->arena);
        where->prev->text.data = s;
    } else {
        //create node
    }
}


node * create_element(node * document, string local_name, string namespace, char * prefix, char * is, bool will_execute_script, void * registry, arena * perm) {
    //TODO: skipping custom registry stuff...

    node * result = new(perm, node);
    result->type = HTML_ELEMENT;
    result->node_document = document;
    result->parent = document;
    result->element.namespace = namespace;
    result->element.local_name = local_name;
    result->element.namespace_prefix = prefix;
    result->element.is = is;
    result->element.custom_element_registry = NULL;
    result->element.custom_element_state = CUSTELEM_UNDEFINED;

    return result;
}

node * create_element_for_token(token t, string namespace, node * target, arena * perm) {
    //TODO: Skipping speculative parsing for now
    // if (active_speculative_parser != NULL) return create_mock_element(...);
    // else create_mock_element(...);
    // ...

    node * document = target->node_document;
    string local_name = t.tag.name;
    char * is = NULL;
    bool will_execute_script = false;
    void * registry = NULL;
    //TODO: add "is" attribute handling whenever I fill in custom elements
    //string is = t.is;
    //void * registry = lookup_custom_elem_registry(target);
    //void * definition = lookup_custom_elem_definition(registry, namespace, local_name, is)
    //bool will_execute_script = definition && !p->is_parsing_fragment;
    //if (will_execute_script) {
    //  document.throw_on_dynamicy_markup_insertion_counter++;
    //  if (is_empty(JS_EXEC_CONTEXT_STACK)) {
    //      microtask_checkpoint();
    //  }
    //  *push(document.relevant_agent.custom_element_reaction_stack) = new_element_queue();
    //}

    node * element = create_element(document, local_name, namespace, NULL, is, will_execute_script, registry, perm);
    for (size i = 0; i < t.attrs.len; i++) {
        //TODO: this might not work
        *push(&element->attributes, perm) = * (node_attr *) &t.attrs.data[i];
    }
    //if (will_execute_script) {
    //  let queue = pop(document.relevant_agent.custom_element_reaction_stack)
    //  invoke_custom_element_reactions(queue)
    //  document.throw_on_dynamicy_markup_insertion_counter--;
    //}

    //
    //more stuff...
    //
    //

    return element;
}

void insertion_mode_initial(parser * p, token t) {
    switch (t.type) {
        case CHARACTER: 
            if (t.character.data == '\t' ||
                    t.character.data == '\n' ||
                    t.character.data == '\f' ||
                    t.character.data == '\r' ||
                    t.character.data == ' ') {
                //ignore
            } else {
                goto _default;
            }
            break;
        case COMMENT:
            insert_comment(t.comment.data, &p->document, p->arena);
            break;
        case DOCTYPE:
            if (!t.doctype.name.exists
                    || !opt_str_equal_c(&t.doctype.name, "html") 
                    || t.doctype.public_id.exists) {
                LOG_ERROR("Unexpected doctype token in initial insert state");
            }
            node n = {0};
            n.type = HTML_DOCTYPE;
            n.doctype.name = *opt_unwrap(&t.doctype.name, string, &String(""));
            n.doctype.public_id = *opt_unwrap(&t.doctype.public_id, string, &String(""));
            n.doctype.system_id = *opt_unwrap(&t.doctype.system_id, string, &String(""));
            *push(&p->document.children, p->arena) = n;

            // TODO: handle quirks mode check better
            if (!s_equal_c(n.doctype.name, "html")
                    && !p->cannot_change_mode
                    && !p->document.document.is_iframe_srcdoc
                    /* TODO: && doctype_matches_a_condition...() */) {
                p->document.document.quirks_mode = true;
            }
            p->insert_mode = BEFORE_HTML;
            break;
        default:
    }
    return;
_default:
    if (!p->document.document.is_iframe_srcdoc) {
        LOG_ERROR("Unexpected document type in initial insert state");
        if (!p->cannot_change_mode) {
            p->document.document.quirks_mode = true;
        }
    }
    p->insert_mode = BEFORE_HTML;
    process(p, t); //reprocess
}

void before_html(parser * p, token t) {
    switch (t.type) {
        case DOCTYPE: 
            LOG_ERROR("Unexpected doctype token in before_html state");
            break;
        case COMMENT:
            insert_comment(t.comment.data, &p->document, p->arena);
            break;
        case CHARACTER:
            if (t.character.data == '\t'
                || t.character.data == '\n'
                || t.character.data == '\f'
                || t.character.data == '\r'
                || t.character.data == ' ') {
                //ignore
                break;
            }
        case START_TAG:
            if (s_equal_c(t.tag.name, "html")) {
                node * elem = create_element_for_token(t, String("html"), &p->document, p->arena);
                *push(&p->open_elem_stack, p->arena) = *elem;
                p->insert_mode = BEFORE_HEAD;
            } else {
                goto _default;
            }
            break;
        case END_TAG: 
            if (s_equal_c(t.tag.name, "head")
                || s_equal_c(t.tag.name, "body")
                || s_equal_c(t.tag.name, "html")
                || s_equal_c(t.tag.name, "br")) {
                goto _default;
            } else {
                //Parse error, ignore
            }
            break;
        default:
            goto _default;
    }

    return;
_default:
    //TODO: create element
    node * elem = create_element_for_token(t, String("html"), &p->document, p->arena);
    *push(&p->document.children, p->arena) = *elem;
    *push(&p->open_elem_stack, p->arena) = *elem;
    p->insert_mode = BEFORE_HEAD;
    process(p, t); //reprocess
}

void before_head(parser * p, token t) {
    switch (t.type) {
        case CHARACTER:
            if (t.character.data == '\t'
                || t.character.data == '\r'
                || t.character.data == '\f'
                || t.character.data == '\n') {
                //ignore
            } else {
                goto _default;
            }
            break;

        case COMMENT:
            insert_comment(t.comment.data, &p->document, p->arena);
            break;

        case DOCTYPE:
            LOG_ERROR("DOCTYPE token in before_head state");
            break;

        case START_TAG:
            if (s_equal_c(t.tag.name, "html")) {
                in_body(p, t);
            } else if (s_equal_c(t.tag.name, "head")) {
                //TODO: insert html element, set pointer
                p->insert_mode = IN_HEAD;
            } else {
                goto _default;
            }
            break;

        case END_TAG:
            if (s_equal_c(t.tag.name, "head")
                || s_equal_c(t.tag.name, "body")
                || s_equal_c(t.tag.name, "html")
                || s_equal_c(t.tag.name, "br")) {
                goto _default;
            } else {
                //Parse error, ignore
                LOG_ERROR("End tag token in before_head state besides head, body, html, or br");
            }
            break;

        default:
            goto _default;
    }

    return;
_default:
    //TODO: new elem, set head pointer
    p->insert_mode = IN_HEAD;
}

void in_head(parser * p, token t) {
    switch(t.type) {
        case CHARACTER:
            if (t.character.data == '\t'
                || t.character.data == '\r'
                || t.character.data == '\f'
                || t.character.data == '\n') {
                //TODO: insert the character
            } else {
                goto _default;
            }
            break;
        case COMMENT:
            insert_comment(t.comment.data, &p->document, p->arena);
            break;
        case DOCTYPE:
            LOG_ERROR("Doctype token in in_head state");
            break;
        case START_TAG:
            if (s_equal_c(t.tag.name, "html")) {
                in_body(p, t);
            } else if (s_equal_c(t.tag.name, "base")
                    || s_equal_c(t.tag.name, "basefont")
                    || s_equal_c(t.tag.name, "bgsound")
                    || s_equal_c(t.tag.name, "link")) {
                //TODO: insert html element
                //pop node stack
                //ack self-closing
            } else if (s_equal_c(t.tag.name, "meta")) {
                //TODO: insert html element
                //pop node stack
                //ack self-closing
                //OTHERWISE do the meta stuff
            } else if (s_equal_c(t.tag.name, "title")) {
                //RCDATA alg
            } else if (s_equal_c(t.tag.name, "noscript")
                    || s_equal_c(t.tag.name, "noframes")
                    || s_equal_c(t.tag.name, "style")) {
                //RAWTEXT alg
            } else if (s_equal_c(t.tag.name, "script")) {
                //TODO:
            } else if (s_equal_c(t.tag.name, "template")) {
                //TODO:
            } else if (s_equal_c(t.tag.name, "head")) {
                //Parse error, ignore
            } else {
                goto _default;
            }
            break;

        case END_TAG:
            {
                if (s_equal_c(t.tag.name, "head")) {
                    //pop node
                    p->insert_mode = AFTER_HEAD;
                } else if (s_equal_c(t.tag.name, "body")
                    || s_equal_c(t.tag.name, "html")
                    || s_equal_c(t.tag.name, "br")) {
                    goto _default;
                } else if (s_equal_c(t.tag.name, "template")) {
                    //TODO:
                } else {
                    //Parse error, ignore
                    LOG_ERROR("End tag token in in_head state besides head, body, html, br, or template");
                }
            }
            break;

        default:
            goto _default;
    }

    return;
_default:
    //TODO: pop head node off of stack
    p->insert_mode = AFTER_HEAD;
    process(p, t); //reprocess
}

void in_head_noscript(parser * p, token t) {
    switch (t.type) {
        case DOCTYPE: 
            //parse error, ignore
            LOG_ERROR("Doctype token in in_head_noscript state");
            break;
        case START_TAG: 
            {
                if (s_equal_c(t.tag.name, "html")) {
                    in_body(p, t);
                } else if (s_equal_c(t.tag.name, "basefont")
                        || s_equal_c(t.tag.name, "bgsound")
                        || s_equal_c(t.tag.name, "link")
                        || s_equal_c(t.tag.name, "meta")
                        || s_equal_c(t.tag.name, "noframes")
                        || s_equal_c(t.tag.name, "style")) {
                    in_head(p, t);
                } else if (s_equal_c(t.tag.name, "head")
                        || s_equal_c(t.tag.name, "noscript")) {
                    //parse error, ignore
                    LOG_ERROR("Unexpected start tag token in in_head_noscript state");
                }
            }
            break;
        case END_TAG:
            {
                if (s_equal_c(t.tag.name, "noscript")) {
                    //pop noscript from stack
                    p->insert_mode = IN_HEAD;
                } else if (s_equal_c(t.tag.name, "br")) {
                    goto _default;
                } else {
                    //parse error, ignore
                    LOG_ERROR("Unexpected end tag token in in_head_noscript state");
                }
            }
            break;
        case CHARACTER:
            if (t.character.data == '\t'
                || t.character.data == '\r'
                || t.character.data == '\f'
                || t.character.data == '\n'
                || t.character.data == ' ') {
                in_head(p, t);
            } else {
                //goto default
            }
            break;
        case COMMENT:
            in_head(p, t);
            break;
        default:
            goto _default;
    }
    return;
_default:
    //parse error
    //pop current noscript elem
    p->insert_mode = IN_HEAD;
    //reprocess token
}

void after_head(parser * p, token t) {
    switch (t.type) {
        case CHARACTER:
            if (t.character.data == '\t'
                || t.character.data == '\r'
                || t.character.data == '\f'
                || t.character.data == '\n'
                || t.character.data == ' ') {
                //insert character
            } else {
                goto _default;
            }
            break;
        case COMMENT:
            insert_comment(t.comment.data, &p->document, p->arena);
            break;
        case DOCTYPE:
            //parse error, ignore
            LOG_ERROR("Unexpected doctype token in after_head state");
            break;
        case START_TAG: 
            {
                if (s_equal_c(t.tag.name, "html")) {
                    in_body(p, t);
                } else if (s_equal_c(t.tag.name, "body")) { 
                    //insert elem
                    //set frameset_ok to "not_ok"
                    p->frameset_ok = false;
                    p->insert_mode = IN_BODY;
                } else if (s_equal_c(t.tag.name, "frameset")) { 
                    //insert elem
                    p->insert_mode = IN_FRAMESET;
                } else if (s_equal_c(t.tag.name, "base")
                        || s_equal_c(t.tag.name, "basefont")
                        || s_equal_c(t.tag.name, "bgsound")
                        || s_equal_c(t.tag.name, "link")
                        || s_equal_c(t.tag.name, "meta")
                        || s_equal_c(t.tag.name, "noframes")
                        || s_equal_c(t.tag.name, "script")
                        || s_equal_c(t.tag.name, "style")
                        || s_equal_c(t.tag.name, "template")
                        || s_equal_c(t.tag.name, "title")) {
                    //parse error
                    //put head->pointer on open elem stack
                    in_head(p, t);
                    //remove head->pointer from stack
                } else if (s_equal_c(t.tag.name, "head")) {
                    //parse error, ignore
                } else {
                    //insert body start tag elem
                    p->insert_mode = IN_BODY;
                    //reprocess token
                }
            }
            break;
        case END_TAG: 
            {
                if (s_equal_c(t.tag.name, "template")) {
                    in_head(p, t);
                } else if (s_equal_c(t.tag.name, "body")
                        || s_equal_c(t.tag.name, "html")
                        || s_equal_c(t.tag.name, "br")) {
                    goto _default;
                } else {
                    //parse error, ignore
                }
            }
            break;
        default:
            goto _default;
    }
    return;
_default:
    //insert body start tag elem
    p->insert_mode = IN_BODY;
    process(p, t); //reprocess
}

void in_body(parser * p, token t) {
    switch (t.type) {
        case CHARACTER:
            switch (t.character.data) {
                case '\0':
                    /* parse error, ignore */
                    LOG_ERROR("Unexpected NULL character token in in_body state");
                    break;

                case '\t':
                case '\f':
                case '\r':
                case '\n':
                    //TODO: reconstruct active formatting elements
                    //insert token character
                    break;

                default:
                    //TODO: reconstruct active formatting elements
                    //insert token character
                    p->frameset_ok = false;
                    break;
            }
            break;
        case COMMENT:
            //TODO: insert comment
            insert_comment(t.comment.data, &p->document, p->arena);
            break;
        case DOCTYPE:
            //parse error, ignore
            LOG_ERROR("Unexpected DOCTYPE token in in_body state");
            break;
        case START_TAG: {
                if (s_equal_c(t.tag.name, "html")) {
                    //parse error
                    LOG_ERROR("Unexpected html start tag token in in_body state");
                    //TODO: if template in open elements, ignore,
                    //else add each attribute that is not already present
                    //to the token on top of the open elements stack
                } else if (s_equal_c(t.tag.name, "base")
                        || s_equal_c(t.tag.name, "basefont")
                        || s_equal_c(t.tag.name, "bgsound")
                        || s_equal_c(t.tag.name, "link")
                        || s_equal_c(t.tag.name, "meta")
                        || s_equal_c(t.tag.name, "noframes")
                        || s_equal_c(t.tag.name, "script")
                        || s_equal_c(t.tag.name, "style")
                        || s_equal_c(t.tag.name, "template")
                        || s_equal_c(t.tag.name, "title")) {
                    in_head(p, t);
                } else if (s_equal_c(t.tag.name, "body")) {
                    //parse error
                    LOG_ERROR("Unexpected body start tag token in in_body state");
                    if ((p->open_elem_stack.len == 1)
                            || p->open_elem_stack.data[1].type != HTML_BODY
                            || stack_has_c(p->open_elem_stack, "template")) {
                        //then ignore
                    } else {
                        p->frameset_ok = false;
                        //  then add not present attrs to current elem on stack
                    }
                } else if (s_equal_c(t.tag.name, "frameset")) {
                    //parse error
                    LOG_ERROR("Unexpected frameset start tag token in in_body state");
                    //if stack_size == 1 || stack[2] != body
                    //  then ignore
                    //else if frameset_ok == false
                    //  then ignore
                    //else
                    //  remove stack[2]
                    //  pop all from stack until 'html'
                    //  insert html
                    p->insert_mode = IN_FRAMESET;
                } else if (s_equal_c(t.tag.name, "address")
                        || s_equal_c(t.tag.name, "article")
                        || s_equal_c(t.tag.name, "aside")
                        || s_equal_c(t.tag.name, "blockquote")
                        || s_equal_c(t.tag.name, "center")
                        || s_equal_c(t.tag.name, "details")
                        || s_equal_c(t.tag.name, "dialog")
                        || s_equal_c(t.tag.name, "dir")
                        || s_equal_c(t.tag.name, "div")
                        || s_equal_c(t.tag.name, "dl")
                        || s_equal_c(t.tag.name, "fieldset")
                        || s_equal_c(t.tag.name, "figcaption")
                        || s_equal_c(t.tag.name, "figure")
                        || s_equal_c(t.tag.name, "footer")
                        || s_equal_c(t.tag.name, "header")
                        || s_equal_c(t.tag.name, "hgroup")
                        || s_equal_c(t.tag.name, "main")
                        || s_equal_c(t.tag.name, "menu")
                        || s_equal_c(t.tag.name, "nav")
                        || s_equal_c(t.tag.name, "ol")
                        || s_equal_c(t.tag.name, "p")
                        || s_equal_c(t.tag.name, "search")
                        || s_equal_c(t.tag.name, "section")
                        || s_equal_c(t.tag.name, "summary")
                        || s_equal_c(t.tag.name, "ul")) {
                    //if stack.has(p)
                    //  close p
                    //insert html elem for token
                } else if (s_equal_c(t.tag.name, "h1")
                        || s_equal_c(t.tag.name, "h2")
                        || s_equal_c(t.tag.name, "h3")
                        || s_equal_c(t.tag.name, "h4")
                        || s_equal_c(t.tag.name, "h5")
                        || s_equal_c(t.tag.name, "h6")) {
                    //if stack.has(p)
                    //  close p
                    //
                    //if current_node == h1 h2 h3 h4 h5 h6
                    //  then parse error
                    //  pop current_node
                    //
                    //insert html_elem for token
                } else if (s_equal_c(t.tag.name, "pre")
                        || s_equal_c(t.tag.name, "listing")) {
                    //if stack.has(p)
                    //  then close p
                    //
                    //insert html_elem for token
                    //
                    //if peek_token() == LF char token
                    //  then get_token() //ignore
                    //
                    //framset_ok = false
                } else if (s_equal_c(t.tag.name, "form")) {
                    //if form_elem_pointer != NULL and !stack.has(template)
                    //  then parse error, ignore
                    //else
                    //  if stack.has(p)
                    //      then close p
                    //  insert html_elem for token
                    //  form_elem_pointer = newly_created_elem
                } else if (s_equal_c(t.tag.name, "li")) {
                    p->frameset_ok = false;
                    //node = current_node
                    //while node is li
                    //  generate implied end tags except li
                    //  if current_node != li
                    //      then parse error
                    //  pop from stack until an li has been popped
                    //  GOTO DONE
                    //if node is 'special' && != address, div, p
                    //  GOTO DONE
                    //else 
                    //  node = stack.top - 1
                    //  GOTO while
                    //DONE:
                    //if stack.has(p) close p
                    //insert html_elem for token
                } else if (s_equal_c(t.tag.name, "dd")
                        || s_equal_c(t.tag.name, "dt")) {
                    p->frameset_ok = false;
                    //node = current_node
                    //LOOP:
                    //if node is dd
                    //  generate implied end tags except dd
                    //  if current_node != dd
                    //      then parse error
                    //  pop from stack until a dd has been popped
                    //  GOTO DONE
                    //if node is dt
                    //  generate implied end tags except dt
                    //  if current_node != dt
                    //      then parse error
                    //  pop from stack until a dt has been popped
                    //  GOTO DONE
                    //if node is special && != address div p
                    //  GOTO DONE
                    //else
                    //  node = stack.top -1
                    //  GOTO LOOP
                    //DONE:
                    //if stack.has(p) close p
                    //insert html_elem for token
                } else if (s_equal_c(t.tag.name, "plaintext")) {
                    //if stack.has(p) close p
                    //insert html elem for token
                    p->lex.state = PLAINTEXT_STATE;
                } else if (s_equal_c(t.tag.name, "button")) {
                    //if stack.has button
                    //  parse error
                    //  generate implied end_tags
                    //  pop from stack until button has been popped
                    //reconstruct active formatting elements
                    //insert html elem for token
                    p->frameset_ok = false;
                }
            }
            break;
        case END_TAG: {
                if (s_equal_c(t.tag.name, "template")) {
                    in_head(p, t);
                } else if (s_equal_c(t.tag.name, "body")) {
                    //if !stack.has(body)
                    //  then parse error, ignore
                    //else if stack.size > 0 && !stack.has 
                    //      dd dt li optgroup option p rb rp rt rtc tbody td 
                    //      tfoot th thead tr body html
                    //  then parse error
                    LOG_ERROR("Unexpected body end tag in in_body state");
                    p->insert_mode = AFTER_BODY;
                } else if (s_equal_c(t.tag.name, "html")) {
                    //if !stack.has(body)
                    //  then parse error, ignore
                    //else if stack.size > 0 && !stack.has 
                    //      dd dt li optgroup option p rb rp rt rtc tbody td 
                    //      tfoot th thead tr body html
                    //  then parse error
                    LOG_ERROR("Unexpected html end tag in in_body state");
                    p->insert_mode = AFTER_BODY;
                    process(p, t); //reprocess
                } else if (s_equal_c(t.tag.name, "address")
                        || s_equal_c(t.tag.name, "article")
                        || s_equal_c(t.tag.name, "aside")
                        || s_equal_c(t.tag.name, "blockquote")
                        || s_equal_c(t.tag.name, "button")
                        || s_equal_c(t.tag.name, "center")
                        || s_equal_c(t.tag.name, "details")
                        || s_equal_c(t.tag.name, "dialog")
                        || s_equal_c(t.tag.name, "dir")
                        || s_equal_c(t.tag.name, "div")
                        || s_equal_c(t.tag.name, "dl")
                        || s_equal_c(t.tag.name, "fieldset")
                        || s_equal_c(t.tag.name, "figcaption")
                        || s_equal_c(t.tag.name, "figure")
                        || s_equal_c(t.tag.name, "footer")
                        || s_equal_c(t.tag.name, "header")
                        || s_equal_c(t.tag.name, "hgroup")
                        || s_equal_c(t.tag.name, "listing")
                        || s_equal_c(t.tag.name, "main")
                        || s_equal_c(t.tag.name, "menu")
                        || s_equal_c(t.tag.name, "nav")
                        || s_equal_c(t.tag.name, "ol")
                        || s_equal_c(t.tag.name, "pre")
                        || s_equal_c(t.tag.name, "search")
                        || s_equal_c(t.tag.name, "section")
                        || s_equal_c(t.tag.name, "summary")
                        || s_equal_c(t.tag.name, "ul")) {
                    //if !stack.has(t.tag.name)
                    //  parse error, ignore
                    //else
                    //  generate implied end_tags
                    //  if current_node != t.tag.name
                    //      parse error
                    //  pop from stack until t.tag.name is popped
                } else if (s_equal_c(t.tag.name, "form")) {
                    //if !stack.has(template)
                    //  node = form_ptr || null
                    //  form_ptr = null
                    //  if node == null || !stack.has(node)
                    //      then parse error, ignore token
                    //      return
                    //  generate implied end_tags
                    //  if current_node != node
                    //      then parse error
                    //  stack.remove(node)
                    //else (if stack.has(template)
                    //  generate implied end tags
                    //  if current_node != form
                    //      then parse error
                    //  pop elements until form has been popped
                } else if (s_equal_c(t.tag.name, "p")) {
                }
            }
            break;
        case END_OF_FILE:
            //if not stack.empty
            //  in_template(p, t);
            //else
            //  if stack.size > 0 && !stack.has 
            //      dd dt li optgroup option p rb rp rt rtc tbody td tfoot th
            //      thead tr body html
            //  then parse error
            //  return (stop parsing)
            break;
        default:
            break;
    }
}

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

