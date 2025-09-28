#include "lexer.h"
#include "lexer_internal.h"
#include "fickit.h"
#include <assert.h>

lexer lexer_init(const char * filename, arena * perm) {
    lexer lex = {0};
    lex.arena = perm;
    lex.state = DATA_STATE;
    lex.input = input_system_init(filename, perm); //FIXME: gross
    return lex;
}

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

string token_to_string(token t, arena * a) {
    switch (t.type) {
        case DOCTYPE: {
                string ret = String("DOCTYPE \"");
                ret = s_cat(ret, *opt_get(&t.doctype.name, string), a);
                ret = s_cat(ret, String("\""), a);
                return ret;
            }
            break;

        case START_TAG: {
                string ret = String("<");
                ret = s_cat(ret, t.tag.name, a);
                for (size i = 0; i < t.attrs.len; i++) {
                    ret = s_cat(ret, String(" "), a);
                    ret = s_cat(ret, t.attrs.data[i].name, a);
                    ret = s_cat(ret, String("=\""), a);
                    ret = s_cat(ret, t.attrs.data[i].value, a);
                    ret = s_cat(ret, String("\""), a);
                }
                ret = s_cat(ret, String(">"), a);
                return ret;
            }
            break;

        case END_TAG: {
                string ret = String("</");
                ret = s_cat(ret, t.tag.name, a);
                ret = s_cat(ret, String(">"), a);
                return ret;
            } 
            break;

        case COMMENT: {
                string ret = String("<!-- ");
                ret = s_cat(ret, t.comment.data, a);
                ret = s_cat(ret, String(" -->"), a);
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
                return s_clone(String("EOF\n"), a); 
            }

        default:
            return String("");
    }

    return String("");
}

void (*state_handlers[])(lexer *) = {
    data_state,
    rcdata_state,
    rawtext_state,
    script_data_state,
    plaintext_state,
    tag_open_state,
    end_tag_open_state,
    tag_name_state,
    rcdata_less_than_sign_state,
    rcdata_end_tag_open_state,
    rcdata_end_tag_name_state,
    rawtext_less_than_sign_state,
    rawtext_end_tag_open_state,
    rawtext_end_tag_name_state,
    script_data_less_than_sign_state,
    script_data_end_tag_open_state,
    script_data_end_tag_name_state,
    script_data_escape_start_state,
    script_data_escape_start_dash_state,
    script_data_escaped_state,
    script_data_escaped_dash_state,
    script_data_escaped_dash_dash_state,
    script_data_escaped_less_than_sign_state,
    script_data_escaped_end_tag_open_state,
    script_data_escaped_end_tag_name_state,
    script_data_double_escape_start_state,
    script_data_double_escaped_state,
    script_data_double_escaped_dash_state,
    script_data_double_escaped_dash_dash_state,
    script_data_double_escaped_less_than_sign_state,
    script_data_double_escape_end_state,
    before_attribute_name_state,
    attribute_name_state,
    after_attribute_name_state,
    before_attribute_value_state,
    attribute_value_double_quoted_state,
    attribute_value_single_quoted_state,
    attribute_value_unquoted_state,
    after_attribute_value_quoted_state,
    self_closing_start_tag_state,
    bogus_comment_state,
    markup_declaration_open_state,
    comment_start_state,
    comment_start_dash_state,
    comment_state,
    comment_less_than_sign_state,
    comment_less_than_sign_bang_state,
    comment_less_than_sign_bang_dash_state,
    comment_less_than_sign_bang_dash_dash_state,
    comment_end_dash_state,
    comment_end_state,
    comment_end_bang_state,
    doctype_state,
    before_doctype_name_state,
    doctype_name_state,
    after_doctype_name_state,
    after_doctype_public_keyword_state,
    before_doctype_public_identifier_state,
    doctype_public_identifier_double_quoted_state,
    doctype_public_identifier_single_quoted_state,
    after_doctype_public_identifier_state,
    between_doctype_public_and_system_identifiers_state,
    after_doctype_system_keyword_state,
    before_doctype_system_identifier_state,
    doctype_system_identifier_double_quoted_state,
    doctype_system_identifier_single_quoted_state,
    after_doctype_system_identifier_state,
    bogus_doctype_state,
    cdata_section_state,
    cdata_section_bracket_state,
    cdata_section_end_state,
    character_reference_state,
    named_character_reference_state,
    ambiguous_ampersand_state,
    numeric_character_reference_state,
    hexadecimal_character_reference_start_state,
    decimal_character_reference_start_state,
    hexadecimal_character_reference_state,
    decimal_character_reference_state,
    numeric_character_reference_end_state,
    invalid_state,
};

token get_token(lexer * lexer) {
    if (lexer->emitted_tokens.len > 0) {
        return lexer->emitted_tokens.data[--lexer->emitted_tokens.len];
    }

    while (lexer->emitted_tokens.len <= 0 && !lexer->parser_pause_flag) {
        if (lexer->parser_pause_flag) {
            break;
        }
        state_handlers[lexer->state](lexer);
    }

    if (lexer->parser_pause_flag) {
        LOG_ERROR("PARSER PAUSED: this shouldn't be happening yet");
        exit(1);
    }

    return lexer->emitted_tokens.data[--lexer->emitted_tokens.len];
}

void emit_token(lexer * lexer, token t) {
    *push_front(&lexer->emitted_tokens, lexer->arena) = t;
}


void clear_temporary_buffer(lexer * lexer) {
    if (lexer->temp_buf.data != NULL) {
        memset(lexer->temp_buf.data, 0, lexer->temp_buf.len);
        lexer->temp_buf.len = 0;
    }
}


void append_to_temp_buffer(lexer * lexer, int c) {
    *push(&lexer->temp_buf, lexer->arena) = c;
}


void emit_tokens_in_temp_buffer(lexer * lexer) {
    for (size i = 0; i < lexer->temp_buf.len; i++) {
        emit_token(lexer, token_character_init(lexer->temp_buf.data[i]));
    }
}


const char* get_temporary_buffer(lexer * lexer) {
    return lexer->temp_buf.data;
}


bool is_part_of_an_attribute(lexer * lexer) {
    return lexer->return_state == ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE
            || lexer->return_state == ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE;
}


void flush_code_points(lexer * lexer) {
    if (is_part_of_an_attribute(lexer)) {
        for (size i = 0; i < lexer->temp_buf.len; i++) {
            append_to_current_tag_token_attribute_value(lexer, lexer->temp_buf.data[i]);
        }
    } else {
        emit_tokens_in_temp_buffer(lexer);
    }
}

void append_to_current_tag_token_comment_data(lexer * lexer, int c) {
    if (lexer->current_token.type != COMMENT) {
        LOG_ERROR("Can't append to comment data for a token of type "xstr(lexer->current_token.type));
    } else {
        *push(&lexer->current_token.comment.data, lexer->arena) = c;
    }
}

bool current_token_is_valid(lexer * lexer) {
    if (lexer->last_start_tag_name.exists 
            && lexer->current_token.type == END_TAG
            && lexer->current_token.tag.name.len > 0) {
        string start_tag = * (string *) lexer->last_start_tag_name.val;
        string end_tag = lexer->current_token.tag.name;
        return s_equal(start_tag, end_tag);
    }
    return false;
}

void set_current_token_identifier(lexer * lexer, const char * val, size len) {
    if (lexer->current_token.type != DOCTYPE) {
        LOG_ERROR("Token of type "xstr(lexer->current_token.type)" does not have an identifier");
        return;
    }

    lexer->current_token.doctype.public_id.exists = true;
    string * s = (string *) lexer->current_token.doctype.public_id.val;
    memset(s->data, 0, s->len);
    s->len = 0;
    for (size i = 0; i < len; i++) {
        *push(s, lexer->arena) = val[i];
    }
}

void append_to_current_tag_token_identifier(lexer * lexer, int c) {
    if (lexer->current_token.type != DOCTYPE) {
        LOG_ERROR("Token of type "xstr(lexer->current_token.type)" does not have an identifier");
        return;
    }
}

bool input_system_empty(input_system * s) {
    return s->len <= 0;
}

void normalize_newlines(string * sb) {
    string temp = {0};
    temp.data = sb->data;
    temp.len = sb->len;

    for (size i = temp.len-1; i > 0; i--) {
        if (temp.data[i] == '\r' && temp.data[i+1] == '\n') {
            for (size j = i; j < temp.len-1; j++) {
                temp.data[j] = temp.data[j+1];
            }
            temp.len -= 1;
        }
    }

    //edge case
    if (temp.data[0] == '\r' && temp.data[1] == '\n') {
            for (size j = 0; j < temp.len-1; j++) {
                temp.data[j] = temp.data[j+1];
            }
            temp.len -= 1;
    }

    for (size i = 0; i < temp.len; i++) {
        if (temp.data[i] == '\r') {
            temp.data[i] = '\n';
        }
    }

    *sb = temp;
}

input_system input_system_init(const char* filename, arena * a) {
    input_system s = {0};

    FILE* f;
    f = fopen(filename, "rb");
    assert(f);

    fseek(f, 0L, SEEK_END);
    size filesize = 0;
    filesize = ftell(f);
    if (filesize < 0) {
        LOG_ERROR("Failed to parse file length");
        exit(EXIT_FAILURE);
    } else if (filesize == 0) {
        LOG_ERROR("File is empty");
        exit(EXIT_SUCCESS);
    }
    fseek(f, 0L, SEEK_SET);

    //TODO: read entire file for now. Change later if it becomes a problem
    string temp = {0};
    temp.data = new(a, char, filesize);
    temp.len = filesize;

    int res = fread(temp.data, 1, filesize, f);
    if (res != filesize) {
        LOG_ERROR("Failed to parse file contents");
        exit(EXIT_FAILURE);
    }
    fclose(f);

    s.buffer = temp;
    s.front = s.buffer.data;
    s.len = s.buffer.len;
    normalize_newlines(&s.buffer);
    return s;
}


int input_system_consume(input_system * s) {
    if (s->len <= 0) {
        LOG_INFO("Input system empty: Emitting EOF");
        return EOF;
    }
    char c = *s->front;
    s->front++;
    s->len--;
    return c;
}

void input_system_reconsume(input_system * s) {
    assert(s->front > s->buffer.data);
    s->front--;
    s->len++;
}

string input_system_peekn(input_system * s, int n, arena * a) {
    string st = {0};
    st.data = new(a, char, n);
    st.len = n;

    int l = n < s->len ? n : s->len;
    for (int i = 0; i < l; i++) {
        st.data[i] = s->front[i];
    }
    return st;
}

char input_system_peek(input_system * s) {
    return *s->front;
}

void set_self_closing_tag_for_current_token(lexer * lexer, bool b) {
    if (lexer->current_token.type == START_TAG) {
        lexer->current_token.tag.self_closing = true;
    } else if (lexer->current_token.type == END_TAG) {
        lexer->current_token.tag.self_closing = true;
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a self closing flag");
    }
}

void append_to_current_tag_token_name(lexer * lexer, int c) {
    if (lexer->current_token.type == START_TAG
        || lexer->current_token.type == END_TAG) {
        *push(&lexer->current_token.tag.name, lexer->arena) = c;
    } else if (lexer->current_token.type == DOCTYPE) {
        opt_str_append(&lexer->current_token.doctype.name, lexer->arena, c);
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a tag name");
    }
}

void start_new_attribute_for_current_tag_token(lexer * lexer) {
    if (lexer->current_token.type == START_TAG) {
        token_attr a = {0};
        *push(&lexer->current_token.attrs, lexer->arena) = a;
    } else if (lexer->current_token.type == END_TAG) {
        token_attr a = {0};
        *push(&lexer->current_token.attrs, lexer->arena) = a;
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a tag name");
    }
}

void append_to_current_tag_token_attribute_name(lexer * lexer, int c) {
    if (lexer->current_token.type == START_TAG) {
        token_attrs a = lexer->current_token.attrs;
        *push(&a.data[a.len-1].name, lexer->arena) = c;
    } else if (lexer->current_token.type == END_TAG) {
        token_attrs a = lexer->current_token.attrs;
        *push(&a.data[a.len-1].name, lexer->arena) = c;
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a tag name");
    }
}

void append_to_current_tag_token_attribute_value(lexer * lexer, int c) {
    if (lexer->current_token.type == START_TAG) {
        token_attrs a = lexer->current_token.attrs;
        *push(&a.data[a.len-1].value, lexer->arena) = c;
    } else if (lexer->current_token.type == END_TAG) {
        token_attrs a = lexer->current_token.attrs;
        *push(&a.data[a.len-1].value, lexer->arena) = c;
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a tag name");
    }
}

void set_doctype_token_force_quirks_flag(lexer * lexer, bool b) {
    if (lexer->current_token.type != DOCTYPE) {
        LOG_ERROR("Token is not a doctype token");
        return;
    }

    lexer->current_token.doctype.force_quirks = b;
}

void set_current_token(lexer * p, token tkn) {
    p->current_token = tkn;
}

token get_current_token(lexer * p) {
    return p->current_token;
}

bool is_named_character(int c) {
    return false; //TODO
}

bool adjusted_current_node() {
    return false; //TODO
}

bool in_html_namespace_placeholder() {
    return false; //TODO
}

/* 
 * attribute name needs to be compared against already created attribute names, 
 * if there are duplicates, it is a duplicate attribute parse error, and the 
 * new attribute needs to be removed from the token
 */
void check_for_duplicate_attributes(lexer * lexer) {
    string new_attr = lexer->current_token.attrs.data[lexer->current_token.attrs.len-1].name;
    for (size i = 0; i < lexer->current_token.attrs.len-1; i++) {
        if (s_equal(new_attr, lexer->current_token.attrs.data[i].name)) {
            LOG_ERROR(xstr(DUPLICATE_ATTRIBUTE_PARSE_ERROR));
            if (lexer->current_token.type == START_TAG) {
                lexer->current_token.attrs.len--;
            } else if (lexer->current_token.type == END_TAG) {
                lexer->current_token.attrs.len--;
            }
            break;
        }
    }
}
