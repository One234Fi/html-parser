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
        (node.type == MATHML_MI_ELEMENT \
        || node.type == MATHML_MO_ELEMENT \
        || node.type == MATHML_MN_ELEMENT \
        || node.type == MATHML_MS_ELEMENT \
        || node.type == MATHML_MTEXT_ELEMENT)


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
            && (!s_equal_c(start_tag_name, "mglyph"))
            && (!s_equal_c(start_tag_name, "malignmark")))
        || (is_mathml_text_integration_point(n)
            && input.type == CHARACTER)
        || (is_mathml_annotation_xml_element(n)
            && input.type == START_TAG
            && (s_equal_c(start_tag_name, "svg")))
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

void insert_comment(string data, node * where, arena * a) {
    node node = {0};
    node.type = HTML_COMMENT;
    node.comment.data = data;
    *push(&where->children, a) = node;
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
                p->insert_mode = BEFORE_HTML;
                process(p, t); //reprocess
            }
            break;
        case COMMENT:
            insert_comment(t.comment.data, &p->document, p->arena);
            break;
        case DOCTYPE:
            if (!t.doctype.name.exists
                    || !opt_str_equal_c(&t.doctype.name, "html") 
                    || t.doctype.public_id.exists) {
                //parse_error
            }
            node n = {0};
            n.type = HTML_DOCTYPE;
            n.doctype.name = *opt_unwrap(&t.doctype.name, string, &String(""));
            n.doctype.public_id = *opt_unwrap(&t.doctype.public_id, string, &String(""));
            n.doctype.system_id = *opt_unwrap(&t.doctype.system_id, string, &String(""));
            *push(&p->document.children, p->arena) = n;

            // TODO: handle quirks mode check better
            if (!s_equal_c(n.doctype.name, "html")) {
                p->document.document.force_quirks = true;
            }
            p->insert_mode = BEFORE_HTML;
            break;
        default:
            //if not an iframe srcdoc doc, then parse error
            p->insert_mode = BEFORE_HTML;
            process(p, t); //reprocess
    }
}

void before_html(parser * p, token t) {
    switch (t.type) {
        case DOCTYPE: 
            //Parse error, ignore
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
            {
            string tag_name = *opt_unwrap(&t.start_tag.name, string, &String(""));
            if (s_equal_c(tag_name, "html")) {
                //create an element
                //  document
                //      -> this
                //then add this to open elem stack
                p->insert_mode = BEFORE_HEAD;
            } else {
                //TODO: same as default
            }
            }
            break;
        case END_TAG: 
            {
            string tag_name = *opt_unwrap(&t.end_tag.name, string, &String(""));
            if (s_equal_c(tag_name, "head")
                || s_equal_c(tag_name, "body")
                || s_equal_c(tag_name, "html")
                || s_equal_c(tag_name, "br")) {
                //TODO: same as default
            } else {
                //Parse error, ignore
            }
            }
            break;
        default:
            //create element, then reprocess token
            p->insert_mode = BEFORE_HEAD;
    }
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
                //TODO: same as default
            }
            break;

        case COMMENT:
            //TODO: insert a comment
            break;

        case DOCTYPE:
            //TODO: Parse error. Ignore
            break;

        case START_TAG:
            {
                string tag_name = *opt_unwrap(&t.start_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "html")) {
                    in_body(p, t);
                } else if (s_equal_c(tag_name, "head")) {
                    //TODO: insert html element, set pointer
                    p->insert_mode = IN_HEAD;
                } else {
                    //TODO: same as default
                }
            }
            break;

        case END_TAG:
            {
                string tag_name = *opt_unwrap(&t.end_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "head")
                    || s_equal_c(tag_name, "body")
                    || s_equal_c(tag_name, "html")
                    || s_equal_c(tag_name, "br")) {
                    //TODO: same as default
                } else {
                    //Parse error, ignore
                }
            }
            break;

        default:
            //TODO: new elem, set head pointer
            p->insert_mode = IN_HEAD;
    }
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
                //TODO: same as default
            }
            break;
        case COMMENT:
            //TODO: insert comment
            break;
        case DOCTYPE:
            //PARSE Error: ignore
            break;
        case START_TAG:
            {
                string tag_name = *opt_unwrap(&t.start_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "html")) {
                    in_body(p, t);
                } else if (s_equal_c(tag_name, "base")
                        || s_equal_c(tag_name, "basefont")
                        || s_equal_c(tag_name, "bgsound")
                        || s_equal_c(tag_name, "link")) {
                    //TODO: insert html element
                    //pop node stack
                    //ack self-closing
                } else if (s_equal_c(tag_name, "meta")) {
                    //TODO: insert html element
                    //pop node stack
                    //ack self-closing
                    //OTHERWISE do the meta stuff
                } else if (s_equal_c(tag_name, "title")) {
                    //RCDATA alg
                } else if (s_equal_c(tag_name, "noscript")
                        || s_equal_c(tag_name, "noframes")
                        || s_equal_c(tag_name, "style")) {
                    //RAWTEXT alg
                } else if (s_equal_c(tag_name, "script")) {
                    //TODO:
                } else if (s_equal_c(tag_name, "template")) {
                    //TODO:
                } else if (s_equal_c(tag_name, "head")) {
                    //Parse error, ignore
                } else {
                    //same as default
                }
            }
            break;

        case END_TAG:
            {
                string tag_name = *opt_unwrap(&t.end_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "head")) {
                    //pop node
                    p->insert_mode = AFTER_HEAD;
                } else if (s_equal_c(tag_name, "body")
                    || s_equal_c(tag_name, "html")
                    || s_equal_c(tag_name, "br")) {
                    //TODO: same as default
                } else if (s_equal_c(tag_name, "template")) {
                    //TODO:
                } else {
                    //Parse error, ignore
                }
            }
            break;

        default:
            //TODO: pop head node off of stack
            p->insert_mode = AFTER_HEAD;
            //TODO: reprocess token
    }
}

void in_head_noscript(parser * p, token t) {
    switch (t.type) {
        case DOCTYPE: 
            //parse error, ignore
            break;
        case START_TAG: 
            {
                string tag_name = *opt_unwrap(&t.start_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "html")) {
                    in_body(p, t);
                } else if (s_equal_c(tag_name, "basefont")
                        || s_equal_c(tag_name, "bgsound")
                        || s_equal_c(tag_name, "link")
                        || s_equal_c(tag_name, "meta")
                        || s_equal_c(tag_name, "noframes")
                        || s_equal_c(tag_name, "style")) {
                    in_head(p, t);
                } else if (s_equal_c(tag_name, "head")
                        || s_equal_c(tag_name, "noscript")) {
                    //parse error, ignore
                }
            }
            break;
        case END_TAG:
            {
                string tag_name = *opt_unwrap(&t.end_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "noscript")) {
                    //pop noscript from stack
                    p->insert_mode = IN_HEAD;
                } else if (s_equal_c(tag_name, "br")) {
                    //TODO: default
                } else {
                    //parse error, ignore
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
            //parse error
            //pop current noscript elem
            p->insert_mode = IN_HEAD;
            //reprocess token
    }
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
                //goto default
            }
            break;
        case COMMENT:
            //TODO: insert comment
            break;
        case DOCTYPE:
            //parse error, ignore
            break;
        case START_TAG: 
            {
                string tag_name = *opt_unwrap(&t.start_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "html")) {
                    in_body(p, t);
                } else if (s_equal_c(tag_name, "body")) { 
                    //insert elem
                    //set frameset_ok to "not_ok"
                    p->frameset_ok = false;
                    p->insert_mode = IN_BODY;
                } else if (s_equal_c(tag_name, "frameset")) { 
                    //insert elem
                    p->insert_mode = IN_FRAMESET;
                } else if (s_equal_c(tag_name, "base")
                        || s_equal_c(tag_name, "basefont")
                        || s_equal_c(tag_name, "bgsound")
                        || s_equal_c(tag_name, "link")
                        || s_equal_c(tag_name, "meta")
                        || s_equal_c(tag_name, "noframes")
                        || s_equal_c(tag_name, "script")
                        || s_equal_c(tag_name, "style")
                        || s_equal_c(tag_name, "template")
                        || s_equal_c(tag_name, "title")) {
                    //parse error
                    //put head->pointer on open elem stack
                    in_head(p, t);
                    //remove head->pointer from stack
                } else if (s_equal_c(tag_name, "head")) {
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
                string tag_name = *opt_unwrap(&t.end_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "template")) {
                    in_head(p, t);
                } else if (s_equal_c(tag_name, "body")
                        || s_equal_c(tag_name, "html")
                        || s_equal_c(tag_name, "br")) {
                    //TODO: default
                } else {
                    //parse error, ignore
                }
            }
            break;
        default:
            //insert body start tag elem
            p->insert_mode = IN_BODY;
            //TODO: reprocess token
    }
}

void in_body(parser * p, token t) {
    switch (t.type) {
        case CHARACTER:
            switch (t.character.data) {
                case '\0':
                    /* parse error, ignore */
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
            break;
        case DOCTYPE:
            //parse error, ignore
            break;
        case START_TAG: {
                string tag_name = *opt_unwrap(&t.start_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "html")) {
                    //parse error
                    //TODO: if template in open elements, ignore,
                    //else add each attribute that is not already present
                    //to the token on top of the open elements stack
                } else if (s_equal_c(tag_name, "base")
                        || s_equal_c(tag_name, "basefont")
                        || s_equal_c(tag_name, "bgsound")
                        || s_equal_c(tag_name, "link")
                        || s_equal_c(tag_name, "meta")
                        || s_equal_c(tag_name, "noframes")
                        || s_equal_c(tag_name, "script")
                        || s_equal_c(tag_name, "style")
                        || s_equal_c(tag_name, "template")
                        || s_equal_c(tag_name, "title")) {
                    in_head(p, t);
                } else if (s_equal_c(tag_name, "body")) {
                    //parse error
                    //if stack_size == 1 || stack[2] != body || stack.has(template)
                    //  then ignore
                    //else 
                    p->frameset_ok = false;
                    //  then add not present attrs to current elem on stack
                } else if (s_equal_c(tag_name, "frameset")) {
                    //parse error
                    //if stack_size == 1 || stack[2] != body
                    //  then ignore
                    //else if frameset_ok == false
                    //  then ignore
                    //else
                    //  remove stack[2]
                    //  pop all from stack until 'html'
                    //  insert html
                    p->insert_mode = IN_FRAMESET;
                } else if (s_equal_c(tag_name, "address")
                        || s_equal_c(tag_name, "article")
                        || s_equal_c(tag_name, "aside")
                        || s_equal_c(tag_name, "blockquote")
                        || s_equal_c(tag_name, "center")
                        || s_equal_c(tag_name, "details")
                        || s_equal_c(tag_name, "dialog")
                        || s_equal_c(tag_name, "dir")
                        || s_equal_c(tag_name, "div")
                        || s_equal_c(tag_name, "dl")
                        || s_equal_c(tag_name, "fieldset")
                        || s_equal_c(tag_name, "figcaption")
                        || s_equal_c(tag_name, "figure")
                        || s_equal_c(tag_name, "footer")
                        || s_equal_c(tag_name, "header")
                        || s_equal_c(tag_name, "hgroup")
                        || s_equal_c(tag_name, "main")
                        || s_equal_c(tag_name, "menu")
                        || s_equal_c(tag_name, "nav")
                        || s_equal_c(tag_name, "ol")
                        || s_equal_c(tag_name, "p")
                        || s_equal_c(tag_name, "search")
                        || s_equal_c(tag_name, "section")
                        || s_equal_c(tag_name, "summary")
                        || s_equal_c(tag_name, "ul")) {
                    //if stack.has(p)
                    //  close p
                    //insert html elem for token
                } else if (s_equal_c(tag_name, "h1")
                        || s_equal_c(tag_name, "h2")
                        || s_equal_c(tag_name, "h3")
                        || s_equal_c(tag_name, "h4")
                        || s_equal_c(tag_name, "h5")
                        || s_equal_c(tag_name, "h6")) {
                    //if stack.has(p)
                    //  close p
                    //
                    //if current_node == h1 h2 h3 h4 h5 h6
                    //  then parse error
                    //  pop current_node
                    //
                    //insert html_elem for token
                } else if (s_equal_c(tag_name, "pre")
                        || s_equal_c(tag_name, "listing")) {
                    //if stack.has(p)
                    //  then close p
                    //
                    //insert html_elem for token
                    //
                    //if peek_token() == LF char token
                    //  then get_token() //ignore
                    //
                    //framset_ok = false
                } else if (s_equal_c(tag_name, "form")) {
                    //if form_elem_pointer != NULL and !stack.has(template)
                    //  then parse error, ignore
                    //else
                    //  if stack.has(p)
                    //      then close p
                    //  insert html_elem for token
                    //  form_elem_pointer = newly_created_elem
                } else if (s_equal_c(tag_name, "li")) {
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
                } else if (s_equal_c(tag_name, "dd")
                        || s_equal_c(tag_name, "dt")) {
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
                } else if (s_equal_c(tag_name, "plaintext")) {
                    //if stack.has(p) close p
                    //insert html elem for token
                    p->lex.state = PLAINTEXT_STATE;
                } else if (s_equal_c(tag_name, "button")) {
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
                string tag_name = *opt_unwrap(&t.end_tag.name, string, &String(""));
                if (s_equal_c(tag_name, "template")) {
                    in_head(p, t);
                } else if (s_equal_c(tag_name, "body")) {
                    //if !stack.has(body)
                    //  then parse error, ignore
                    //else if stack.size > 0 && !stack.has 
                    //      dd dt li optgroup option p rb rp rt rtc tbody td 
                    //      tfoot th thead tr body html
                    //  then parse error
                    p->insert_mode = AFTER_BODY;
                } else if (s_equal_c(tag_name, "html")) {
                    //if !stack.has(body)
                    //  then parse error, ignore
                    //else if stack.size > 0 && !stack.has 
                    //      dd dt li optgroup option p rb rp rt rtc tbody td 
                    //      tfoot th thead tr body html
                    //  then parse error
                    p->insert_mode = AFTER_BODY;
                    process(p, t); //reprocess
                } else if (s_equal_c(tag_name, "address")
                        || s_equal_c(tag_name, "article")
                        || s_equal_c(tag_name, "aside")
                        || s_equal_c(tag_name, "blockquote")
                        || s_equal_c(tag_name, "button")
                        || s_equal_c(tag_name, "center")
                        || s_equal_c(tag_name, "details")
                        || s_equal_c(tag_name, "dialog")
                        || s_equal_c(tag_name, "dir")
                        || s_equal_c(tag_name, "div")
                        || s_equal_c(tag_name, "dl")
                        || s_equal_c(tag_name, "fieldset")
                        || s_equal_c(tag_name, "figcaption")
                        || s_equal_c(tag_name, "figure")
                        || s_equal_c(tag_name, "footer")
                        || s_equal_c(tag_name, "header")
                        || s_equal_c(tag_name, "hgroup")
                        || s_equal_c(tag_name, "listing")
                        || s_equal_c(tag_name, "main")
                        || s_equal_c(tag_name, "menu")
                        || s_equal_c(tag_name, "nav")
                        || s_equal_c(tag_name, "ol")
                        || s_equal_c(tag_name, "pre")
                        || s_equal_c(tag_name, "search")
                        || s_equal_c(tag_name, "section")
                        || s_equal_c(tag_name, "summary")
                        || s_equal_c(tag_name, "ul")) {
                    //if !stack.has(tag_name)
                    //  parse error, ignore
                    //else
                    //  generate implied end_tags
                    //  if current_node != tag_name
                    //      parse error
                    //  pop from stack until tag_name is popped
                } else if (s_equal_c(tag_name, "form")) {
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
                } else if (s_equal_c(tag_name, "p")) {
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

