/*
 * State machine for html tokenization
 */

#include "tokenizer.h"
#include "types/arena.h"
#include "types/opt.h"
#include "types/str.h"
#include "types/types.h"
#include "types/vector.h"
#include "common.h"
#include "input.h"
#include "named_character_references.h"
#include "error.h"
#include "code_point_types.h"
#include "parser/token.h"
#include "types/opt.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

const char * TOKENIZER_STATE_STRINGS[] = {
    DEFINE_STATE_TYPES(MAKE_ENUM_STRINGS)
};

parser parser_init(arena * a, input_system i) {
    parser p = {0};
    p.arena = a;
    p.input = i;
    p.state = DATA_STATE;
    return p;
}

#define UNICODE_REPLACEMENT_CHAR 0xEF


void clear_temporary_buffer(parser * p) {
    memset(p->temp_buf.data, 0, p->temp_buf.len);
    p->temp_buf.len = 0;
    p->temp_buf.cap = 0;
}

void append_to_temp_buffer(parser * p, int c) {
    *push(&p->temp_buf, p->arena) = c;
}

void emit_tokens_in_temp_buffer(parser * p) {
    for (size i = 0; i < p->temp_buf.len; i++) {
        emit_token(token_character_init(p->temp_buf.data[i]));
    }
}

const char* get_temporary_buffer(parser * p) {
    return p->temp_buf.data;
}

bool is_part_of_an_attribute(parser * p) {
    return p->return_state == ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE
            || p->return_state == ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE;
}

void append_to_current_tag_token_attribute_value(parser * p, int c); //FIXME: Forward declare

void flush_code_points(parser * p) {
    if (is_part_of_an_attribute(p)) {
        for (size i = 0; i < p->temp_buf.len; i++) {
            append_to_current_tag_token_attribute_value(p, p->temp_buf.data[i]);
        }
    } else {
        emit_tokens_in_temp_buffer(p);
    }
}

void append_to_current_tag_token_comment_data(parser * p, int c) {
    if (p->current_token.type != COMMENT) {
        LOG_ERROR("Can't append to comment data for a token of type "xstr(p->current_token.type));
    } else {
        *push(&p->current_token.comment.data, p->arena) = c;
    }
}

bool current_token_is_valid(parser * p) {
    if (p->last_start_tag_name.exists 
            && p->current_token.type == END_TAG
            && p->current_token.end_tag.name.exists) {
        string start_tag = * (string *) p->last_start_tag_name.val;
        string end_tag = * (string *) p->current_token.end_tag.name.val;
        return string_equal(start_tag, end_tag);
    }
    return false;
}

void set_current_token_identifier(parser * p, const char * val, size len) {
    if (p->current_token.type != DOCTYPE) {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have an identifier");
        return;
    }

    p->current_token.doctype.public_id.exists = true;
    string * s = (string *) p->current_token.doctype.public_id.val;
    memset(s->data, 0, s->len);
    s->len = 0;
    for (size i = 0; i < len; i++) {
        *push(s, p->arena) = val[i];
    }
}

void append_to_current_tag_token_identifier(parser * p, int c) {
    if (p->current_token.type != DOCTYPE) {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have an identifier");
        return;
    }
}

extern bool adjusted_current_node();
extern bool in_html_namespace();
extern void return_state(parser * p);
extern bool is_named_character(int c);

void set_doctype_token_force_quirks_flag(parser * p, bool b) {
    if (p->current_token.type != DOCTYPE) {
        LOG_ERROR("Token is not a doctype token");
        return;
    }

    p->current_token.doctype.force_quirks = b;
}


/* 
 * attribute name needs to be compared against already created attribute names, 
 * if there are duplicates, it is a duplicate attribute parse error, and the 
 * new attribute needs to be removed from the token
 */
void check_for_duplicate_attributes(parser * p) {
    attrs list;
    if (p->current_token.type == START_TAG) {
        list = p->current_token.start_tag.attributes;
    } else if (p->current_token.type == END_TAG) {
        list = p->current_token.end_tag.attributes;
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have attributes");
        return;
    }

    string new_attr = list.data[list.len-1].name;
    for (size i = 0; i < p->current_token.start_tag.attributes.len-1; i++) {
        if (string_equal(new_attr, list.data[i].name)) {
            LOG_ERROR(xstr(DUPLICATE_ATTRIBUTE_PARSE_ERROR));
            if (p->current_token.type == START_TAG) {
                p->current_token.start_tag.attributes.len--;
            } else if (p->current_token.type == END_TAG) {
                p->current_token.end_tag.attributes.len--;
            }
            break;
        }
    }
}


void set_state(parser * p, enum TOKENIZER_STATE_TYPE state) {
    p->state = state;
}

void return_state(parser * p) {
    p->state = p->return_state;
}

void set_return_state(parser * p, enum TOKENIZER_STATE_TYPE state) {
    p->return_state = state;
}

enum TOKENIZER_STATE_TYPE get_state(parser * p) {
    return p->state;
}

void set_current_token(parser * p, token tkn) {
    p->current_token = tkn;
}

token get_current_token(parser * p) {
    return p->current_token;
}

void set_self_closing_tag_for_current_token(parser * p, bool b) {
    if (p->current_token.type == START_TAG) {
        p->current_token.start_tag.self_closing = true;
    } else if (p->current_token.type == END_TAG) {
        p->current_token.end_tag.self_closing = true;
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a self closing flag");
    }
}

void append_to_current_tag_token_name(parser * p, int c) {
    if (p->current_token.type == START_TAG) {
        opt_str_append(&p->current_token.start_tag.name, p->arena, c);
    } else if (p->current_token.type == END_TAG) {
        opt_str_append(&p->current_token.end_tag.name, p->arena, c);
    } else if (p->current_token.type == DOCTYPE) {
        opt_str_append(&p->current_token.doctype.name, p->arena, c);
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a tag name");
    }
}

void start_new_attribute_for_current_tag_token(parser * p) {
    if (p->current_token.type == START_TAG) {
        attr a = {0};
        *push(&p->current_token.start_tag.attributes, p->arena) = a;
    } else if (p->current_token.type == END_TAG) {
        attr a = {0};
        *push(&p->current_token.end_tag.attributes, p->arena) = a;
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a tag name");
    }
}

void append_to_current_tag_token_attribute_name(parser * p, int c) {
    if (p->current_token.type == START_TAG) {
        attrs a = p->current_token.start_tag.attributes;
        *push(&a.data[a.len-1].name, p->arena) = c;
    } else if (p->current_token.type == END_TAG) {
        attrs a = p->current_token.end_tag.attributes;
        *push(&a.data[a.len-1].name, p->arena) = c;
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a tag name");
    }
}

void append_to_current_tag_token_attribute_value(parser * p, int c) {
    if (p->current_token.type == START_TAG) {
        attrs a = p->current_token.start_tag.attributes;
        *push(&a.data[a.len-1].value, p->arena) = c;
    } else if (p->current_token.type == END_TAG) {
        attrs a = p->current_token.end_tag.attributes;
        *push(&a.data[a.len-1].value, p->arena) = c;
    } else {
        LOG_ERROR("Token of type "xstr(p->current_token.type)" does not have a tag name");
    }
}

//state handlers
void data_state(parser * p);
void rcdata_state(parser * p);
void rawtext_state(parser * p);
void script_data_state(parser * p);
void plaintext_state(parser * p);
void tag_open_state(parser * p);
void end_tag_open_state(parser * p);
void tag_name_state(parser * p);
void rcdata_less_than_sign_state(parser * p);
void rcdata_end_tag_open_state(parser * p);
void rcdata_end_tag_name_state(parser * p);
void rawtext_less_than_sign_state(parser * p);
void rawtext_end_tag_open_state(parser * p);
void rawtext_end_tag_name_state(parser * p);
void script_data_less_than_sign_state(parser * p);
void script_data_end_tag_open_state(parser * p);
void script_data_end_tag_name_state(parser * p);
void script_data_escape_start_state(parser * p);
void script_data_escape_start_dash_state(parser * p);
void script_data_escaped_state(parser * p);
void script_data_escaped_dash_state(parser * p);
void script_data_escaped_dash_dash_state(parser * p);
void script_data_escaped_less_than_sign_state(parser * p);
void script_data_escaped_end_tag_open_state(parser * p);
void script_data_escaped_end_tag_name_state(parser * p);
void script_data_double_escape_start_state(parser * p);
void script_data_double_escaped_state(parser * p);
void script_data_double_escaped_dash_state(parser * p);
void script_data_double_escaped_dash_dash_state(parser * p);
void script_data_double_escaped_less_than_sign_state(parser * p);
void script_data_double_escape_end_state(parser * p);
void before_attribute_name_state(parser * p);
void attribute_name_state(parser * p);
void after_attribute_name_state(parser * p);
void before_attribute_value_state(parser * p);
void attribute_value_double_quoted_state(parser * p);
void attribute_value_single_quoted_state(parser * p);
void attribute_value_unquoted_state(parser * p);
void after_attribute_value_quoted_state(parser * p);
void self_closing_start_tag_state(parser * p);
void bogus_comment_state(parser * p);
void markup_declaration_open_state(parser * p);
void comment_start_state(parser * p);
void comment_start_dash_state(parser * p);
void comment_state(parser * p);
void comment_less_than_sign_state(parser * p);
void comment_less_than_sign_bang_state(parser * p);
void comment_less_than_sign_bang_dash_state(parser * p);
void comment_less_than_sign_bang_dash_dash_state(parser * p);
void comment_end_dash_state(parser * p);
void comment_end_state(parser * p);
void comment_end_bang_state(parser * p);
void doctype_state(parser * p);
void before_doctype_name_state(parser * p);
void doctype_name_state(parser * p);
void after_doctype_name_state(parser * p);
void after_doctype_public_keyword_state(parser * p);
void before_doctype_public_identifier_state(parser * p);
void doctype_public_identifier_double_quoted_state(parser * p);
void doctype_public_identifier_single_quoted_state(parser * p);
void after_doctype_public_identifier_state(parser * p);
void between_doctype_public_and_system_identifiers_state(parser * p);
void after_doctype_system_keyword_state(parser * p);
void before_doctype_system_identifier_state(parser * p);
void doctype_system_identifier_double_quoted_state(parser * p);
void doctype_system_identifier_single_quoted_state(parser * p);
void after_doctype_system_identifier_state(parser * p);
void bogus_doctype_state(parser * p);
void cdata_section_state(parser * p);
void cdata_section_bracket_state(parser * p);
void cdata_section_end_state(parser * p);
void character_reference_state(parser * p);
void named_character_reference_state(parser * p);
void ambiguous_ampersand_state(parser * p);
void numeric_character_reference_state(parser * p);
void hexadecimal_character_reference_start_state(parser * p);
void decimal_character_reference_start_state(parser * p);
void hexadecimal_character_reference_state(parser * p);
void decimal_character_reference_state(parser * p);
void numeric_character_reference_end_state(parser * p);
void invalid_state(parser * p);

void (*state_handlers[])(parser *) = {
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

void execute(parser * p) {
    if (p->parser_pause_flag) {
        return;
    }
    size l = snprintf(NULL, 0, "STATE: %s", TOKENIZER_STATE_STRINGS[p->state]);
    char buf[l+1];
    buf[l] = 0;
    sprintf(buf, "STATE: %s", TOKENIZER_STATE_STRINGS[p->state]);
    LOG_INFO(buf);
    state_handlers[get_state(p)](p);
}

void data_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '&': 
            set_return_state(p, DATA_STATE); 
            set_state(p, CHARACTER_REFERENCE_STATE);
            break;
        case '<': 
            set_state(p, TAG_OPEN_STATE); 
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            emit_token(token_character_init(c));
            break;
        case EOF:
		    p->eof_emitted = true;
            emit_token(token_eof_init());
            break;
        default:
            emit_token(token_character_init(c));
    }
}

void rcdata_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '&':
            set_return_state(p, DATA_STATE); 
            set_state(p, CHARACTER_REFERENCE_STATE);
            break;
        case '<':
            set_state(p, RCDATA_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true;
            emit_token(token_eof_init());
            break;
        default:
            emit_token(token_character_init(c));
    }
}

void rawtext_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '<':
            set_state(p, RAWTEXT_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true;
            emit_token(token_eof_init());
            break;
        default:
            emit_token(token_character_init(c));
    }
}

void script_data_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '<':
            set_state(p, SCRIPT_DATA_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true;
            emit_token(token_eof_init());
            break;
        default:
            emit_token(token_character_init(c));
    }
}

void plaintext_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true;
            emit_token(token_eof_init());
            break;
        default:
            emit_token(token_character_init(c));
    }
}

void tag_open_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '!':
            set_state(p, MARKUP_DECLARATION_OPEN_STATE);
            break;
        case '/':
            set_state(p, END_TAG_OPEN_STATE);
            break;
        case '?':
            LOG_ERROR(xstr(UNEXPECTED_QUESTION_MARK_INSTEAD_OF_TAG_NAME_PARSE_ERROR));
            set_current_token(p, token_comment_init());
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_COMMENT_STATE);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_BEFORE_TAG_NAME_PARSE_ERROR));
            emit_token(token_character_init('<'));
            emit_token(token_eof_init());
            break;
        default:
            if (is_ascii_alpha(c)) {
                set_current_token(p, token_start_tag_init());
                input_system_reconsume(&p->input);
                set_state(p, TAG_NAME_STATE);
                return;
            } else {
                LOG_ERROR(xstr(INVALID_FIRST_CHARACTER_OF_TAG_NAME_PARSE_ERROR));
                emit_token(token_character_init('<'));
                input_system_reconsume(&p->input);
                set_state(p, DATA_STATE);
            }
    }
}

void end_tag_open_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '>':
            LOG_ERROR(xstr(MISSING_END_TAG_NAME_PARSE_ERROR));
            set_state(p, DATA_STATE);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_BEFORE_TAG_NAME_PARSE_ERROR));
            emit_token(token_character_init('/'));
            emit_token(token_eof_init());
            break;
        default:
            if (is_ascii_alpha(c)) {
                set_current_token(p, token_end_tag_init());
                input_system_reconsume(&p->input);
                set_state(p, TAG_NAME_STATE);
            } else {
                LOG_ERROR(xstr(INVALID_FIRST_CHARACTER_OF_TAG_NAME_PARSE_ERROR));
                set_current_token(p, token_comment_init());
                input_system_reconsume(&p->input);
                set_state(p, BOGUS_COMMENT_STATE);
            }
    }
}

void tag_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            break;
        case '/':
            set_state(p, SELF_CLOSING_START_TAG_STATE);
            break;
        case '>':
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p)); //emit current tag token
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_name(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(p, tolower(c));
            } else {
                append_to_current_tag_token_name(p, c);
            }
    }
}

void rcdata_less_than_sign_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (c == '/') {
        clear_temporary_buffer(p);
        set_state(p, RCDATA_END_TAG_OPEN_STATE);
    } else {
        emit_token(token_character_init('<'));
        input_system_reconsume(&p->input);
        set_state(p, RCDATA_STATE);
    }
}

void rcdata_end_tag_open_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alpha(c)) {
        set_current_token(p, token_eof_init());
        input_system_reconsume(&p->input);
        set_state(p, RCDATA_END_TAG_NAME_STATE);
    } else {
        emit_token(token_character_init('<'));
        emit_token(token_character_init('/'));
        input_system_reconsume(&p->input);
        set_state(p, RCDATA_STATE);
    }
}

void rcdata_end_tag_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid(p)) {
                set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, RCDATA_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid(p)) {
                set_state(p, SELF_CLOSING_START_TAG_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, RCDATA_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid(p)) {
                set_state(p, DATA_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, RCDATA_STATE);
            }
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(p, tolower(c));
                append_to_temp_buffer(p, c);
            } else if (is_ascii_lower_alpha(c)) {
                append_to_current_tag_token_name(p, c);
                append_to_temp_buffer(p, c);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, RCDATA_STATE);
            }
    }
}

void rawtext_less_than_sign_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (c == '/') {
        clear_temporary_buffer(p);
        set_state(p, RAWTEXT_END_TAG_OPEN_STATE);
    } else {
        emit_token(token_character_init('<'));
        input_system_reconsume(&p->input);
        set_state(p, RAWTEXT_STATE);
    }
}

void rawtext_end_tag_open_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alpha(c)) {
        set_current_token(p, token_eof_init());
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_END_TAG_NAME_STATE);
    } else {
        emit_token(token_character_init('/'));
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_STATE);
    }
}

void rawtext_end_tag_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\f':
        case '\n':
        case ' ':
            if (current_token_is_valid(p)) {
                set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, RAWTEXT_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid(p)) {
                set_state(p, SELF_CLOSING_START_TAG_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, DATA_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid(p)) {
                set_state(p, DATA_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, DATA_STATE);
            }
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(p, tolower(c));
                append_to_temp_buffer(p, c);
            } else if (is_ascii_lower_alpha(c)) {
                append_to_current_tag_token_name(p, c);
                append_to_temp_buffer(p, c);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, RAWTEXT_STATE);
            }
    }
}

void script_data_less_than_sign_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '/':
            clear_temporary_buffer(p);
            set_state(p, SCRIPT_DATA_END_TAG_OPEN_STATE);
            break;
        case '!':
            set_state(p, SCRIPT_DATA_ESCAPE_START_STATE);
            emit_token(token_character_init('<'));
            emit_token(token_character_init('!'));
            break;
        default:
            emit_token(token_character_init('<'));
            set_state(p, SCRIPT_DATA_STATE);
    }
}

void script_data_end_tag_open_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alpha(c)) {
        set_current_token(p, token_end_tag_init());
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_END_TAG_NAME_STATE);
    } else {
        emit_token(token_character_init('<'));
        emit_token(token_character_init('/'));
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_STATE);
    }
}

void script_data_end_tag_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid(p)) { //current token should be an end tag
                set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid(p)) { //current token should be an end tag
                set_state(p, SELF_CLOSING_START_TAG_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid(p)) { //current token should be an end tag
                set_state(p, DATA_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_STATE);
            }
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(p, tolower(c));
                append_to_temp_buffer(p, c);
            } else if (is_ascii_lower_alpha(c)) {
                append_to_current_tag_token_name(p, c);
                append_to_temp_buffer(p, c);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_STATE);
            }
    }
}

void script_data_escape_start_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (c == '-') {
        set_state(p, SCRIPT_DATA_ESCAPE_START_DASH_STATE);
        emit_token(token_character_init('-'));
    } else {
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_STATE);
    }
}

void script_data_escape_start_dash_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (c == '-') {
        set_state(p, SCRIPT_DATA_ESCAPED_DASH_DASH_STATE);
        emit_token(token_character_init('-'));
    } else {
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_STATE);
    }
}

void script_data_escaped_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, SCRIPT_DATA_ESCAPED_DASH_STATE);
            emit_token(token_character_init('-'));
            break;
        case '<':
            set_state(p, SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true; 
            LOG_ERROR(xstr(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            emit_token(token_character_init(c));
    }
}

void script_data_escaped_dash_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-': 
            set_state(p, SCRIPT_DATA_ESCAPED_DASH_DASH_STATE);
            break;
        case '<': 
            set_state(p, SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true; 
            LOG_ERROR(xstr(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            emit_token(token_character_init(c));
    }
}

void script_data_escaped_dash_dash_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-': 
            emit_token(token_character_init('-'));
            break;
        case '<': 
            set_state(p, SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE);
            break;
        case '>': 
            set_state(p, SCRIPT_DATA_STATE);
            emit_token(token_character_init('>'));
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true; 
            LOG_ERROR(xstr(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            emit_token(token_character_init(c));
    }
}

void script_data_escaped_less_than_sign_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '/':
            clear_temporary_buffer(p);
            set_state(p, SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE);
            break;
        default:
            if (is_ascii_alpha(c)) {
                clear_temporary_buffer(p);
                emit_token(token_character_init('<'));
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE);
            } else {
                emit_token(token_character_init('<'));
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
    }
}

void script_data_escaped_end_tag_open_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alpha(c)) {
        set_current_token(p, token_end_tag_init());
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE);
    } else {
        emit_token(token_character_init('<'));
        emit_token(token_character_init('/'));
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_ESCAPED_STATE);
    }
}

void script_data_escaped_end_tag_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid(p)) {
                set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid(p)) {
                set_state(p, SELF_CLOSING_START_TAG_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid(p)) {
                set_state(p, DATA_STATE);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(p, tolower(c));
                append_to_temp_buffer(p, c);
            } else if (is_ascii_lower_alpha(c)) {
                append_to_current_tag_token_name(p, c);
                append_to_temp_buffer(p, c);
            } else {
                emit_token(token_character_init('<'));
                emit_token(token_character_init('/'));
                emit_tokens_in_temp_buffer(p); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
    }
}

void script_data_double_escape_start_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
            if (strncmp(get_temporary_buffer(p), "script", 6) == 0) {
                set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            } else {
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
            emit_token(token_character_init(c));
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_temp_buffer(p, tolower(c));
                emit_token(token_character_init(c));
            } else if (is_ascii_lower_alpha(c)) {
                append_to_temp_buffer(p, c);
                emit_token(token_character_init(c));
            } else {
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
    }
}

void script_data_double_escaped_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '-':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE);
            emit_token(token_character_init('-'));
            break;
        case '<':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE);
            emit_token(token_character_init('<'));
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            emit_token(token_character_init(c));
    }
}

void script_data_double_escaped_dash_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '-':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE);
            emit_token(token_character_init('-'));
            break;
        case '<':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE);
            emit_token(token_character_init('<'));
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            emit_token(token_character_init(c));
    }
}

void script_data_double_escaped_dash_dash_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            emit_token(token_character_init('-'));
            break;
        case '<':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE);
            emit_token(token_character_init('<'));
            break;
        case '>':
            set_state(p, SCRIPT_DATA_STATE);
            emit_token(token_character_init('>'));
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            emit_token(token_character_init(UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            emit_token(token_character_init(c));
    }
}

void script_data_double_escaped_less_than_sign_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '/':
            clear_temporary_buffer(p);
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE);
            emit_token(token_character_init('/'));
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
    }
}

void script_data_double_escape_end_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
            if (strncmp(get_temporary_buffer(p), "script", 6) == 0) {
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            } else {
                set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            }
            emit_token(token_character_init(c));
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_temp_buffer(p, tolower(c));
                emit_token(token_character_init(c));
            } else if (is_ascii_lower_alpha(c)) {
                append_to_temp_buffer(p, c);
                emit_token(token_character_init(c));
            } else {
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            }
    }
}

void before_attribute_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
        case EOF:
            input_system_reconsume(&p->input);
            set_state(p, AFTER_ATTRIBUTE_NAME_STATE);
            break;
        case '=':
            LOG_ERROR(xstr(UNEXPECTED_EQUALS_SIGN_BEFORE_ATTRIBUTE_NAME_PARSE_ERROR));
            start_new_attribute_for_current_tag_token(p);
            append_to_current_tag_token_attribute_name(p, c);
            set_state(p, ATTRIBUTE_NAME_STATE);
            break;
        default:
            start_new_attribute_for_current_tag_token(p);
            input_system_reconsume(&p->input);
            set_state(p, ATTRIBUTE_NAME_STATE);
    }
}

void attribute_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
        case EOF:
            input_system_reconsume(&p->input);
            set_state(p, AFTER_ATTRIBUTE_NAME_STATE);
            check_for_duplicate_attributes(p);
            break;
        case '=':
            set_state(p, BEFORE_ATTRIBUTE_VALUE_STATE);
            check_for_duplicate_attributes(p);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_attribute_name(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case '"':
        case '\'':
        case '<':
            LOG_ERROR(xstr(UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR));
            append_to_current_tag_token_attribute_name(p, c);
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_attribute_name(p, tolower(c));
            } else {
                append_to_current_tag_token_attribute_name(p, c);
            }
    }
}

void after_attribute_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore these characters
            break;
        case '/':
            set_state(p, SELF_CLOSING_START_TAG_STATE);
            break;
        case '=':
            set_state(p, BEFORE_ATTRIBUTE_VALUE_STATE);
            break;
        case '>':
            set_state(p, DATA_STATE);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            start_new_attribute_for_current_tag_token(p);
            input_system_reconsume(&p->input);
            set_state(p, ATTRIBUTE_NAME_STATE);
    }
}

void before_attribute_value_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore these characters
            break;
        case '"':
            set_state(p, ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_state(p, ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(MISSING_ATTRIBUTE_VALUE_PARSE_ERROR));
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, ATTRIBUTE_VALUE_UNQUOTED_STATE);
    }
}

void attribute_value_double_quoted_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '"':
            set_state(p, AFTER_ATTRIBUTE_VALUE_QUOTED_STATE);
            break;
        case '&':
            set_return_state(p, ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE);
            set_state(p, CHARACTER_REFERENCE_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_attribute_value(p, c);
    }
}

void attribute_value_single_quoted_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\'':
            set_state(p, AFTER_ATTRIBUTE_VALUE_QUOTED_STATE);
            break;
        case '&':
            set_return_state(p, ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE);
            set_state(p, CHARACTER_REFERENCE_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_attribute_value(p, c);
    }
}

void attribute_value_unquoted_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            break;
        case '&':
            set_return_state(p, ATTRIBUTE_VALUE_UNQUOTED_STATE);
            set_state(p, CHARACTER_REFERENCE_STATE);
            break;
        case '>':
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case '"':
        case '\'':
        case '<':
        case '=':
        case '`':
            LOG_ERROR(xstr(UNEXPECTED_CHARACTER_IN_UNQUOTED_ATTRIBUTE_VALUE_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(p, c);
            break;
        case EOF:
		    p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_attribute_value(p, c);
    }
}

void after_attribute_value_quoted_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            break;
        case '/':
            set_state(p, SELF_CLOSING_START_TAG_STATE);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_WHITESPACE_BETWEEN_ATTRIBUTES_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
    }
}

void self_closing_start_tag_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            set_self_closing_tag_for_current_token(p, true);
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_TAG_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(UNEXPECTED_SOLIDUS_IN_TAG_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
    }
}

void bogus_comment_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_comment_data(p, UNICODE_REPLACEMENT_CHAR);
            break;
        default:
            append_to_current_tag_token_comment_data(p, c);
    }
}

void markup_declaration_open_state(parser * p) {
    string buf = input_system_peekn(&p->input, 7, p->arena);

    if (buf.data[0] == '-' && buf.data[1] == '-') {
        input_system_consume(&p->input);
        input_system_consume(&p->input);
        set_current_token(p, token_comment_init());
        set_state(p, COMMENT_START_STATE);
        return;
    } 

    if (buf.len == 7) {
        if (strncmp(buf.data, "[CDATA[", buf.len) == 0) {
            for (uint32_t i = 0; i < buf.len; i++) {
                input_system_consume(&p->input);
            }

            if (adjusted_current_node() && !in_html_namespace()) {
                set_state(p, CDATA_SECTION_STATE);
            } else {
                LOG_ERROR(xstr(CDATA_IN_HTML_CONTENT_PARSE_ERROR));
                set_current_token(p, token_comment_init());
                append_to_current_tag_token_comment_data(p, '[');
                append_to_current_tag_token_comment_data(p, 'C');
                append_to_current_tag_token_comment_data(p, 'D');
                append_to_current_tag_token_comment_data(p, 'A');
                append_to_current_tag_token_comment_data(p, 'T');
                append_to_current_tag_token_comment_data(p, 'A');
                append_to_current_tag_token_comment_data(p, '[');
                set_state(p, BOGUS_COMMENT_STATE);
            }
            return;
        }

        char lowercase_buf[buf.len] = {};
        for (uint32_t i = 0; i < buf.len; i++) {
            lowercase_buf[i] = tolower(buf.data[i]);
        }
        if (strncmp(lowercase_buf, "doctype", buf.len) == 0) {
            for (uint32_t i = 0; i < buf.len; i++) {
                input_system_consume(&p->input);
            }
            set_state(p, DOCTYPE_STATE);
            return;
        }
    }

    LOG_ERROR(xstr(INCORRECTLY_OPENED_COMMENT_PARSE_ERROR));
    set_current_token(p, token_comment_init());
    set_state(p, BOGUS_COMMENT_STATE);
}

void comment_start_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, COMMENT_START_DASH_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR));
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_start_dash_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, COMMENT_END_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR));
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_COMMENT_PARSE_ERROR));
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_comment_data(p, '-');
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '<':
            append_to_current_tag_token_comment_data(p, c);
            set_state(p, COMMENT_LESS_THAN_SIGN_STATE);
            break;
        case '-':
            set_state(p, COMMENT_END_DASH_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_comment_data(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_COMMENT_PARSE_ERROR));
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_comment_data(p, c);
    }
}

void comment_less_than_sign_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '!':
            append_to_current_tag_token_comment_data(p, c);
            set_state(p, COMMENT_LESS_THAN_SIGN_BANG_STATE);
            break;
        case '<':
            append_to_current_tag_token_comment_data(p, c);
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_less_than_sign_bang_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, COMMENT_LESS_THAN_SIGN_BANG_DASH_STATE);
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_less_than_sign_bang_dash_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH_STATE);
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_END_DASH_STATE);
    }
}

void comment_less_than_sign_bang_dash_dash_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
        case EOF:
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_END_STATE);
            break;
        default:
            LOG_ERROR(xstr(NESTED_COMMENT_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_END_STATE);
    }
}

void comment_end_dash_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, COMMENT_END_STATE);
            break;
        case EOF:
            LOG_ERROR(xstr(EOF_IN_COMMENT_PARSE_ERROR));
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_comment_data(p, '-');
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_end_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case '!':
            set_state(p, COMMENT_END_BANG_STATE);
            break;
        case '-':
            append_to_current_tag_token_comment_data(p, '-');
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_COMMENT_PARSE_ERROR));
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_comment_data(p, '-');
            append_to_current_tag_token_comment_data(p, '-');
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_end_bang_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            append_to_current_tag_token_comment_data(p, '-');
            append_to_current_tag_token_comment_data(p, '-');
            append_to_current_tag_token_comment_data(p, '!');
            set_state(p, COMMENT_END_DASH_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(INCORRECTLY_CLOSED_COMMENT_PARSE_ERROR));
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_COMMENT_PARSE_ERROR));
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_comment_data(p, '-');
            append_to_current_tag_token_comment_data(p, '-');
            append_to_current_tag_token_comment_data(p, '!');
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void doctype_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, BEFORE_DOCTYPE_NAME_STATE);
            break;
        case '>':
            input_system_reconsume(&p->input);
            set_state(p, BEFORE_DOCTYPE_NAME_STATE);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_COMMENT_PARSE_ERROR));
            set_current_token(p, token_doctype_init());
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_WHITESPACE_BEFORE_DOCTYPE_NAME_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, BEFORE_DOCTYPE_NAME_STATE);
    }
}

void before_doctype_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            // intentionally ignore character
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_current_token(p, token_doctype_init());
            append_to_current_tag_token_name(p, UNICODE_REPLACEMENT_CHAR);
            set_state(p, DOCTYPE_NAME_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(MISSING_DOCTYPE_NAME_PARSE_ERROR));
            set_current_token(p, token_doctype_init());
            set_doctype_token_force_quirks_flag(p, true);
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_current_token(p, token_doctype_init());
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                set_current_token(p, token_doctype_init());
                append_to_current_tag_token_name(p, tolower(c));
                set_state(p, DOCTYPE_NAME_STATE);
            } else {
                set_current_token(p, token_doctype_init());
                append_to_current_tag_token_name(p, c);
                set_state(p, DOCTYPE_NAME_STATE);
            }
    }
}

void doctype_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, AFTER_DOCTYPE_NAME_STATE);
            break;
        case '>':
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_name(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(p, tolower(c));
            } else {
                append_to_current_tag_token_name(p, c);
            }
    }
}

void after_doctype_name_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            // intentionally ignore character
            return;
        case '>':
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            return;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            return;
    }

    string temp = input_system_peekn(&p->input, 5, p->arena);
    char buf[6] = {};
    buf[0] = tolower(c);
    buf[1] = tolower(temp.data[0]);
    buf[2] = tolower(temp.data[1]);
    buf[3] = tolower(temp.data[2]);
    buf[4] = tolower(temp.data[3]);
    buf[5] = tolower(temp.data[4]);

    if (strncmp(buf, "public", 6) == 0) {
        for (size_t i = 0; i < 5; i++) {
            input_system_consume(&p->input);
        }
        set_state(p, AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE);
        return;
    }  

    if (strncmp(buf, "system", 6) == 0) {
        for (size_t i = 0; i < 5; i++) {
            input_system_consume(&p->input);
        }
        set_state(p, AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE);
        return;
    }

    LOG_ERROR(xstr(INVALID_CHARACTER_SEQUENCE_AFTER_DOCTYPE_NAME_PARSE_ERROR));
    set_doctype_token_force_quirks_flag(p, true);
    input_system_reconsume(&p->input);
    set_state(p, BOGUS_DOCTYPE_STATE);
}

void after_doctype_public_keyword_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE);
            break;
        case '"':
            LOG_ERROR(xstr(MISSING_WHITESPACE_AFTER_DOCTYPE_PUBLIC_KEYWORD_PARSE_ERROR));
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            LOG_ERROR(xstr(MISSING_WHITESPACE_AFTER_DOCTYPE_PUBLIC_KEYWORD_PARSE_ERROR));
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(MISSING_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void before_doctype_public_identifier_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore character
            break;
        case '"':
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(MISSING_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void doctype_public_identifier_double_quoted_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '"':
            set_state(p, AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            break;
        default:
            append_to_current_tag_token_identifier(p, c);
    }
}

void doctype_public_identifier_single_quoted_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '"':
            set_state(p, AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_identifier(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case '>':
            LOG_ERROR(xstr(ABRUPT_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_identifier(p, c);
    }
}

void after_doctype_public_identifier_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE);
            break;
        case '>':
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case '"':
            LOG_ERROR(xstr(MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_PARSE_ERROR));
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            LOG_ERROR(xstr(MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_PARSE_ERROR));
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void between_doctype_public_and_system_identifiers_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            // intentionally ignore character
            break;
        case '>':
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case '"':
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void after_doctype_system_keyword_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE);
            break;
        case '"':
            LOG_ERROR(xstr(MISSING_WHITESPACE_AFTER_DOCTYPE_SYSTEM_KEYWORD_PARSE_ERROR));
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            LOG_ERROR(xstr(MISSING_WHITESPACE_AFTER_DOCTYPE_SYSTEM_KEYWORD_PARSE_ERROR));
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(MISSING_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void before_doctype_system_identifier_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore character
            break;
        case '"':
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_current_token_identifier(p, "", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            LOG_ERROR(xstr(MISSING_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            LOG_ERROR(xstr(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
            break;
    }
}

void doctype_system_identifier_double_quoted_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '"':
            set_state(p, AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_identifier(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case '>':
            LOG_ERROR(xstr(ABRUPT_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_identifier(p, c);
    }
}

void doctype_system_identifier_single_quoted_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\'':
            set_state(p, AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE);
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_identifier(p, UNICODE_REPLACEMENT_CHAR);
            break;
        case '>':
            LOG_ERROR(xstr(ABRUPT_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(p, true);
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
        default:
            append_to_current_tag_token_identifier(p, c);
    }
}

void after_doctype_system_identifier_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore character
            break;
        case '>':
            set_state(p, DATA_STATE);
            emit_token(get_current_token(p));
            break;
        default:
            LOG_ERROR(xstr(UNEXPECTED_CHARACTER_AFTER_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void bogus_doctype_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            set_state(p, DATA_STATE);
            emit_token(token_doctype_init());
            break;
        case '\0':
            LOG_ERROR(xstr(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            // intentionally ignore character
            break;
        case EOF:
            p->eof_emitted = true;
            emit_token(get_current_token(p));
            emit_token(token_eof_init());
            break;
            // otherwise intentionally ignore character
    }
}

void cdata_section_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case ']':
            set_state(p, CDATA_SECTION_BRACKET_STATE);
            break;
        case EOF:
            p->eof_emitted = true;
            LOG_ERROR(xstr(EOF_IN_CDATA_PARSE_ERROR));
            emit_token(token_eof_init());
            break;
        default:
            emit_token(token_character_init(c));
    }
}

void cdata_section_bracket_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case ']':
            set_state(p, CDATA_SECTION_END_STATE);
            break;
        default:
            emit_token(token_character_init(']'));
            input_system_reconsume(&p->input);
            set_state(p, CDATA_SECTION_STATE);
    }
}

void cdata_section_end_state(parser * p) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case ']':
            emit_token(token_character_init(']'));
            break;
        case '>':
            set_state(p, DATA_STATE);
            break;
        default:
            emit_token(token_character_init(']'));
            emit_token(token_character_init(']'));
            input_system_reconsume(&p->input);
            set_state(p, CDATA_SECTION_STATE);
    }
}

void character_reference_state(parser * p) {
    clear_temporary_buffer(p);
    append_to_temp_buffer(p, '&');
    int c = input_system_consume(&p->input);
    switch (c) {
        case '#':
            append_to_temp_buffer(p, c);
            set_state(p, NUMERIC_CHARACTER_REFERENCE_STATE);
            break;
        default:
            if (is_ascii_alphanumeric(c)) {
                input_system_reconsume(&p->input);
                set_state(p, NAMED_CHARACTER_REFERENCE_STATE);
            } else {
                flush_code_points(p);
                input_system_reconsume(&p->input);
                return_state(p);
            }
    }
}

//TODO: double check the logic of this one 
void named_character_reference_state(parser * p) {
    while (is_named_character(input_system_peek(&p->input))) {
        int c = input_system_consume(&p->input);
        append_to_temp_buffer(p, c);

        if (is_part_of_an_attribute(p) 
                && c != ';' 
                && !isalnum(c)
                && (input_system_peek(&p->input) == '=' || isalnum(input_system_peek(&p->input)) )) {
            flush_code_points(p);
            return_state(p);
            return;
        } else {
            if (c != ';') {
                LOG_ERROR(xstr(MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE_PARSE_ERROR));
            }
            clear_temporary_buffer(p);
            interpret_character_reference_name(p, c);
            flush_code_points(p);
            return_state(p);
            return;
        }
    }
    flush_code_points(p);
    set_state(p, AMBIGUOUS_AMPERSAND_STATE);
}

void ambiguous_ampersand_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alphanumeric(c)) {
        if (is_part_of_an_attribute(p)) {
            append_to_current_tag_token_attribute_value(p, c);
        } else {
            emit_token(token_character_init(c));
        }
    } else if (c == ';') {
        LOG_ERROR(xstr(UNKNOWN_NAMED_CHARACTER_REFERENCE_PARSE_ERROR));
        input_system_reconsume(&p->input);
        return_state(p);
    } else {
        input_system_reconsume(&p->input);
        return_state(p);
    }
}

void numeric_character_reference_state(parser * p) {
    p->char_ref_code = 0;
    int c = input_system_consume(&p->input);
    switch (c) {
        case 'x':
        case 'X':
            append_to_temp_buffer(p, c);
            set_state(p, HEXADECIMAL_CHARACTER_REFERENCE_START_STATE);
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, DECIMAL_CHARACTER_REFERENCE_START_STATE);
    }
}

void hexadecimal_character_reference_start_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_hex_digit(c)) {
        input_system_reconsume(&p->input);
        set_state(p, HEXADECIMAL_CHARACTER_REFERENCE_START_STATE);
    } else {
        LOG_ERROR(xstr(ABSENCE_OF_DIGITS_IN_NUMERIC_CHARACTER_REFERENCE_PARSE_ERROR));
        flush_code_points(p);
        input_system_reconsume(&p->input);
        return_state(p);
    }
}

void decimal_character_reference_start_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_digit(c)) {
        input_system_reconsume(&p->input);
        set_state(p, DECIMAL_CHARACTER_REFERENCE_STATE);
    } else {
        LOG_ERROR(xstr(ABSENCE_OF_DIGITS_IN_NUMERIC_CHARACTER_REFERENCE_PARSE_ERROR));
        flush_code_points(p);
        input_system_reconsume(&p->input);
        return_state(p);
    }
}

void hexadecimal_character_reference_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_digit(c)) {
        int curr_as_numeric = c - 0x0030;
        int ref_code = p->char_ref_code * 16;
        ref_code += curr_as_numeric;
        p->char_ref_code = ref_code;
    } else if (is_ascii_upper_hex_digit(c)) {
        int curr_as_numeric = c - 0x0037;
        int ref_code = p->char_ref_code * 16;
        ref_code += curr_as_numeric;
        p->char_ref_code = ref_code;
    } else if (is_ascii_lower_hex_digit(c)) {
        int curr_as_numeric = c - 0x0057;
        int ref_code = p->char_ref_code * 16;
        ref_code += curr_as_numeric;
        p->char_ref_code = ref_code;
    } else if (c == ';') {
        set_state(p, NUMERIC_CHARACTER_REFERENCE_STATE);
    } else {
        LOG_ERROR(xstr(MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE_PARSE_ERROR));
        input_system_reconsume(&p->input);
        set_state(p, NUMERIC_CHARACTER_REFERENCE_END_STATE);
    }
}


void decimal_character_reference_state(parser * p) {
    int c = input_system_consume(&p->input);
    if (is_ascii_digit(c)) {
        int curr_as_numeric = c - 0x0030;
        int ref_code = p->char_ref_code * 10;
        ref_code += curr_as_numeric;
        p->char_ref_code = ref_code;
    } else if (c == ';') {
        set_state(p, NUMERIC_CHARACTER_REFERENCE_END_STATE);
    } else {
        LOG_ERROR(xstr(MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE_PARSE_ERROR));
        input_system_reconsume(&p->input);
        set_state(p, NUMERIC_CHARACTER_REFERENCE_END_STATE);
    }
}

void numeric_character_reference_end_state(parser * p) {
    int char_ref = p->char_ref_code;
    if (char_ref == 0x00) {
        LOG_ERROR(xstr(NULL_CHARACTER_REFERENCE_PARSE_ERROR));
        p->char_ref_code = 0xFFFD;
    } else if (char_ref > 0x10FFFF) {
        LOG_ERROR(xstr(CHARACTER_REFERENCE_OUTSIDE_OF_UNICODE_RANGE_PARSE_ERROR));
        p->char_ref_code = 0xFFFD;
    } else if (is_surrogate(char_ref)) {
        LOG_ERROR(xstr(SURROGATE_CHARACTER_REFERENCE_PARSE_ERROR));
        p->char_ref_code = 0xFFFD;
    } else if (is_non_char(char_ref)) {
        LOG_ERROR(xstr(NONCHARACTER_CHARACTER_REFERENCE_PARSE_ERROR));
    } else if (char_ref == 0x0D 
            || (is_control(char_ref) && !is_ascii_whitespace(char_ref))) {
        LOG_ERROR(xstr(CONTROL_CHARACTER_REFERENCE_PARSE_ERROR));
    } else {
#define map_case(num, code_point) case num: p->char_ref_code = code_point; break
        switch (char_ref) {
            map_case(0x80, 0x20AC);
            map_case(0x82, 0x201A);
            map_case(0x83, 0x0192);
            map_case(0x84, 0x201E);
            map_case(0x85, 0x2026);
            map_case(0x86, 0x2020);
            map_case(0x87, 0x2021);
            map_case(0x88, 0x02C6);
            map_case(0x89, 0x2030);
            map_case(0x8A, 0x0160);
            map_case(0x8B, 0x2039);
            map_case(0x8C, 0x0152);
            map_case(0x8E, 0x017D);
            map_case(0x91, 0x2018);
            map_case(0x92, 0x2019);
            map_case(0x93, 0x201C);
            map_case(0x94, 0x201D);
            map_case(0x95, 0x2022);
            map_case(0x96, 0x2013);
            map_case(0x97, 0x2014);
            map_case(0x98, 0x02DC);
            map_case(0x99, 0x2122);
            map_case(0x9A, 0x0161);
            map_case(0x9B, 0x203A);
            map_case(0x9C, 0x0153);
            map_case(0x9E, 0x017E);
            map_case(0x9F, 0x0178);
        }
#undef map_case
        clear_temporary_buffer(p);
        append_to_temp_buffer(p, char_ref);
        flush_code_points(p);
        return_state(p);
    }
}

void invalid_state(parser * p) {
    LOG_ERROR(xstr(INVALID_TOKENIZER_STATE_ERROR));
}
