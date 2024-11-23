/*
 * State machine for html tokenization
 */

#include "tokenizer.h"
#include "arena/arena.h"
#include "common.h"
#include "error.h"
#include "input.h"
#include "code_point_types.h"
#include "token/token.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>


typedef struct parser {
    token * current_token;
    enum TOKENIZER_STATE_TYPE state;
    enum TOKENIZER_STATE_TYPE return_state;
    enum INSERTION_MODE_TYPE insertion_mode;
    bool parser_pause_flag;
    opt_str last_start_tag_name;
    arena * arena;
    input_system input;
} parser;

parser * parser_init(arena * a) {
    return NULL;
}

#define UNICODE_REPLACEMENT_CHAR 0xEF


//TODO: unimplemented and uncategorized stubs
extern void append_to_current_tag_token_name(int c);
extern void clear_temporary_buffer();
extern void append_to_temp_buffer(int c);
extern void emit_tokens_in_temp_buffer();
extern bool current_token_is_valid();
extern const char* get_temporary_buffer();
extern void start_new_attribute_for_current_tag_token(); 
extern void append_to_current_tag_token_attribute_name(int c);
extern void append_to_current_tag_token_attribute_value(int c);
extern void set_self_closing_tag_for_current_token(bool b);
extern void append_to_current_tag_token_comment_data(int c);
extern bool adjusted_current_node();
extern bool in_html_namespace();
extern void set_doctype_token_force_quirks_flag (bool b);
extern void set_current_token_identifier(const char* val, size_t len);
extern void append_to_current_tag_token_identifier(int c);
extern void return_state(parser * p);
extern bool is_named_character(int c);
extern bool is_part_of_an_attribute();
extern void interpret_character_reference_name();
extern void set_character_reference_code(int c);
extern int get_character_reference_code();

/* 
 * attribute name needs to be compared against already created attribute names, 
 * if there are duplicates, it is a duplicate attribute parse error, and the 
 * new attribute needs to be removed from the token
 */
extern void check_for_duplicate_attributes();


//for int i in temp buffer, append i to current attribute
extern void flush_code_points();



extern void set_state(parser * p, enum TOKENIZER_STATE_TYPE state);
extern void set_return_state(parser * p, enum TOKENIZER_STATE_TYPE state);
extern enum TOKENIZER_STATE_TYPE get_state(parser * p);

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

void set_current_token(parser * p, token * tkn) {
    p->current_token = tkn;
}

token * get_current_token(parser * p) {
    return p->current_token;
}

void emit_current_token(parser * p, token_type expected) {
    token * t = get_current_token(p);
    if (expected < TOKEN_TYPE_COUNT && expected != t->type) {
        LOG_WARN("Emitting unexpected token type!");
    } else if (expected == TOKEN_TAG && !(t->type == START_TAG || t->type == END_TAG)) {
        LOG_WARN("Emitting unexpected token type!");
    }

    emit_token(get_current_token(p));
}

void character_token_emit(const int c) {
    char str [20] = {};
    snprintf(str, 20, "CHARACTER TOKEN: %c", c);
    LOG_INFO(str);
}

void eof_token_emit() {
    LOG_INFO("EOF TOKEN");
}

void append_to_current_tag_token_name(int c) {
}

char* state_to_string(enum TOKENIZER_STATE_TYPE state) {
    switch (state) {
        case DATA_STATE: return "DATA_STATE";
        case RCDATA_STATE: return "RCDATA_STATE";
        case RAWTEXT_STATE: return "RAWTEXT_STATE";
        case SCRIPT_DATA_STATE: return "SCRIPT_DATA_STATE";
        case PLAINTEXT_STATE: return "PLAINTEXT_STATE";
        case TAG_OPEN_STATE: return "TAG_OPEN_STATE";
        case END_TAG_OPEN_STATE: return "END_TAG_OPEN_STATE";
        case TAG_NAME_STATE: return "TAG_NAME_STATE";
        case RCDATA_LESS_THAN_SIGN_STATE: return "RCDATA_LESS_THAN_SIGN_STATE";
        case RCDATA_END_TAG_OPEN_STATE: return "RCDATA_END_TAG_OPEN_STATE";
        case RCDATA_END_TAG_NAME_STATE: return "RCDATA_END_TAG_NAME_STATE";
        case RAWTEXT_LESS_THAN_SIGN_STATE: return "RAWTEXT_LESS_THAN_SIGN_STATE";
        case RAWTEXT_END_TAG_OPEN_STATE: return "RAWTEXT_END_TAG_OPEN_STATE";
        case RAWTEXT_END_TAG_NAME_STATE: return "RAWTEXT_END_TAG_NAME_STATE";
        case SCRIPT_DATA_LESS_THAN_SIGN_STATE: return "SCRIPT_DATA_LESS_THAN_SIGN_STATE";
        case SCRIPT_DATA_END_TAG_OPEN_STATE: return "SCRIPT_DATA_END_TAG_OPEN_STATE";
        case SCRIPT_DATA_END_TAG_NAME_STATE: return "SCRIPT_DATA_END_TAG_NAME_STATE";
        case SCRIPT_DATA_ESCAPE_START_STATE: return "SCRIPT_DATA_ESCAPE_START_STATE";
        case SCRIPT_DATA_ESCAPE_START_DASH_STATE: return "SCRIPT_DATA_ESCAPE_START_DASH_STATE";
        case SCRIPT_DATA_ESCAPED_STATE: return "SCRIPT_DATA_ESCAPED_STATE";
        case SCRIPT_DATA_ESCAPED_DASH_STATE: return "SCRIPT_DATA_ESCAPED_DASH_STATE";
        case SCRIPT_DATA_ESCAPED_DASH_DASH_STATE: return "SCRIPT_DATA_ESCAPED_DASH_DASH_STATE";
        case SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE: return "SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE";
        case SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE: return "SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE";
        case SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE: return "SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE";
        case SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE: return "SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE";
        case SCRIPT_DATA_DOUBLE_ESCAPED_STATE: return "SCRIPT_DATA_DOUBLE_ESCAPED_STATE";
        case SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE: return "SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE";
        case SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE: return "SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE";
        case SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE: return "SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE";
        case SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE: return "SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE";
        case BEFORE_ATTRIBUTE_NAME_STATE: return "BEFORE_ATTRIBUTE_NAME_STATE";
        case ATTRIBUTE_NAME_STATE: return "ATTRIBUTE_NAME_STATE";
        case AFTER_ATTRIBUTE_NAME_STATE: return "AFTER_ATTRIBUTE_NAME_STATE";
        case BEFORE_ATTRIBUTE_VALUE_STATE: return "BEFORE_ATTRIBUTE_VALUE_STATE";
        case ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE: return "ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE";
        case ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE: return "ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE";
        case ATTRIBUTE_VALUE_UNQUOTED_STATE: return "ATTRIBUTE_VALUE_UNQUOTED_STATE";
        case AFTER_ATTRIBUTE_VALUE_QUOTED_STATE: return "AFTER_ATTRIBUTE_VALUE_QUOTED_STATE";
        case SELF_CLOSING_START_TAG_STATE: return "SELF_CLOSING_START_TAG_STATE";
        case BOGUS_COMMENT_STATE: return "BOGUS_COMMENT_STATE";
        case MARKUP_DECLARATION_OPEN_STATE: return "MARKUP_DECLARATION_OPEN_STATE";
        case COMMENT_START_STATE: return "COMMENT_START_STATE";
        case COMMENT_START_DASH_STATE: return "COMMENT_START_DASH_STATE";
        case COMMENT_STATE: return "COMMENT_STATE";
        case COMMENT_LESS_THAN_SIGN_STATE: return "COMMENT_LESS_THAN_SIGN_STATE";
        case COMMENT_LESS_THAN_SIGN_BANG_STATE: return "COMMENT_LESS_THAN_SIGN_BANG_STATE";
        case COMMENT_LESS_THAN_SIGN_BANG_DASH_STATE: return "COMMENT_LESS_THAN_SIGN_BANG_DASH_STATE";
        case COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH_STATE: return "COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH_STATE";
        case COMMENT_END_DASH_STATE: return "COMMENT_END_DASH_STATE";
        case COMMENT_END_STATE: return "COMMENT_END_STATE";
        case COMMENT_END_BANG_STATE: return "COMMENT_END_BANG_STATE";
        case DOCTYPE_STATE: return "DOCTYPE_STATE";
        case BEFORE_DOCTYPE_NAME_STATE: return "BEFORE_DOCTYPE_NAME_STATE";
        case DOCTYPE_NAME_STATE: return "DOCTYPE_NAME_STATE";
        case AFTER_DOCTYPE_NAME_STATE: return "AFTER_DOCTYPE_NAME_STATE";
        case AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE: return "AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE";
        case BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE: return "BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE";
        case DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE: return "DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE";
        case DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE: return "DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE";
        case AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE: return "AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE";
        case BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE: return "BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE";
        case AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE: return "AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE";
        case BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE: return "BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE";
        case DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE: return "DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE";
        case DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE: return "DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE";
        case AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE: return "AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE";
        case BOGUS_DOCTYPE_STATE: return "BOGUS_DOCTYPE_STATE";
        case CDATA_SECTION_STATE: return "CDATA_SECTION_STATE";
        case CDATA_SECTION_BRACKET_STATE: return "CDATA_SECTION_BRACKET_STATE";
        case CDATA_SECTION_END_STATE: return "CDATA_SECTION_END_STATE";
        case CHARACTER_REFERENCE_STATE: return "CHARACTER_REFERENCE_STATE";
        case NAMED_CHARACTER_REFERENCE_STATE: return "NAMED_CHARACTER_REFERENCE_STATE";
        case AMBIGUOUS_AMPERSAND_STATE: return "AMBIGUOUS_AMPERSAND_STATE";
        case NUMERIC_CHARACTER_REFERENCE_STATE: return "NUMERIC_CHARACTER_REFERENCE_STATE";
        case HEXADECIMAL_CHARACTER_REFERENCE_START_STATE: return "HEXADECIMAL_CHARACTER_REFERENCE_START_STATE";
        case DECIMAL_CHARACTER_REFERENCE_START_STATE: return "DECIMAL_CHARACTER_REFERENCE_START_STATE";
        case HEXADECIMAL_CHARACTER_REFERENCE_STATE: return "HEXADECIMAL_CHARACTER_REFERENCE_STATE";
        case DECIMAL_CHARACTER_REFERENCE_STATE: return "DECIMAL_CHARACTER_REFERENCE_STATE";
        case NUMERIC_CHARACTER_REFERENCE_END_STATE: return "NUMERIC_CHARACTER_REFERENCE_END_STATE";
        default: return "INVALID_TOKENIZER_STATE";
    }
}

void LOG_CURRENT_STATE(parser * p) {
    char* str = state_to_string(get_state(p));
    char outstr[strlen(str) + 12] = {};
    snprintf(outstr, strlen(str) + 12, "STATE: %s", str);
    LOG_INFO(outstr);
}

//state handlers
void data_state(parser * p, arena * a);
void rcdata_state(parser * p, arena * a);
void rawtext_state(parser * p, arena * a);
void script_data_state(parser * p, arena * a);
void plaintext_state(parser * p, arena * a);
void tag_open_state(parser * p, arena * a);
void end_tag_open_state(parser * p, arena * a);
void tag_name_state(parser * p, arena * a);
void rcdata_less_than_sign_state(parser * p, arena * a);
void rcdata_end_tag_open_state(parser * p, arena * a);
void rcdata_end_tag_name_state(parser * p, arena * a);
void rawtext_less_than_sign_state(parser * p, arena * a);
void rawtext_end_tag_open_state(parser * p, arena * a);
void rawtext_end_tag_name_state(parser * p, arena * a);
void script_data_less_than_sign_state(parser * p, arena * a);
void script_data_end_tag_open_state(parser * p, arena * a);
void script_data_end_tag_name_state(parser * p, arena * a);
void script_data_escape_start_state(parser * p, arena * a);
void script_data_escape_start_dash_state(parser * p, arena * a);
void script_data_escaped_state(parser * p, arena * a);
void script_data_escaped_dash_state(parser * p, arena * a);
void script_data_escaped_dash_dash_state(parser * p, arena * a);
void script_data_escaped_less_than_sign_state(parser * p, arena * a);
void script_data_escaped_end_tag_open_state(parser * p, arena * a);
void script_data_escaped_end_tag_name_state(parser * p, arena * a);
void script_data_double_escape_start_state(parser * p, arena * a);
void script_data_double_escaped_state(parser * p, arena * a);
void script_data_double_escaped_dash_state(parser * p, arena * a);
void script_data_double_escaped_dash_dash_state(parser * p, arena * a);
void script_data_double_escaped_less_than_sign_state(parser * p, arena * a);
void script_data_double_escape_end_state(parser * p, arena * a);
void before_attribute_name_state(parser * p, arena * a);
void attribute_name_state(parser * p, arena * a);
void after_attribute_name_state(parser * p, arena * a);
void before_attribute_value_state(parser * p, arena * a);
void attribute_value_double_quoted_state(parser * p, arena * a);
void attribute_value_single_quoted_state(parser * p, arena * a);
void attribute_value_unquoted_state(parser * p, arena * a);
void after_attribute_value_quoted_state(parser * p, arena * a);
void self_closing_start_tag_state(parser * p, arena * a);
void bogus_comment_state(parser * p, arena * a);
void markup_declaration_open_state(parser * p, arena * a);
void comment_start_state(parser * p, arena * a);
void comment_start_dash_state(parser * p, arena * a);
void comment_state(parser * p, arena * a);
void comment_less_than_sign_state(parser * p, arena * a);
void comment_less_than_sign_bang_state(parser * p, arena * a);
void comment_less_than_sign_bang_dash_state(parser * p, arena * a);
void comment_less_than_sign_bang_dash_dash_state(parser * p, arena * a);
void comment_end_dash_state(parser * p, arena * a);
void comment_end_state(parser * p, arena * a);
void comment_end_bang_state(parser * p, arena * a);
void doctype_state(parser * p, arena * a);
void before_doctype_name_state(parser * p, arena * a);
void doctype_name_state(parser * p, arena * a);
void after_doctype_name_state(parser * p, arena * a);
void after_doctype_public_keyword_state(parser * p, arena * a);
void before_doctype_public_identifier_state(parser * p, arena * a);
void doctype_public_identifier_double_quoted_state(parser * p, arena * a);
void doctype_public_identifier_single_quoted_state(parser * p, arena * a);
void after_doctype_public_identifier_state(parser * p, arena * a);
void between_doctype_public_and_system_identifiers_state(parser * p, arena * a);
void after_doctype_system_keyword_state(parser * p, arena * a);
void before_doctype_system_identifier_state(parser * p, arena * a);
void doctype_system_identifier_double_quoted_state(parser * p, arena * a);
void doctype_system_identifier_single_quoted_state(parser * p, arena * a);
void after_doctype_system_identifier_state(parser * p, arena * a);
void bogus_doctype_state(parser * p, arena * a);
void cdata_section_state(parser * p, arena * a);
void cdata_section_bracket_state(parser * p, arena * a);
void cdata_section_end_state(parser * p, arena * a);
void character_reference_state(parser * p, arena * a);
void named_character_reference_state(parser * p, arena * a);
void ambiguous_ampersand_state(parser * p, arena * a);
void numeric_character_reference_state(parser * p, arena * a);
void hexadecimal_character_reference_start_state(parser * p, arena * a);
void decimal_character_reference_start_state(parser * p, arena * a);
void hexadecimal_character_reference_state(parser * p, arena * a);
void decimal_character_reference_state(parser * p, arena * a);
void numeric_character_reference_end_state(parser * p, arena * a);

void execute(parser * p, arena * a) {
    if (p->parser_pause_flag) {
        return;
    }
    LOG_CURRENT_STATE(p);
    switch (get_state(p)) {
        case DATA_STATE: data_state(p, a); break;
        case RCDATA_STATE: rcdata_state(p, a); break;
        case RAWTEXT_STATE: rawtext_state(p, a); break;
        case SCRIPT_DATA_STATE: script_data_state(p, a); break;
        case PLAINTEXT_STATE: plaintext_state(p, a); break;
        case TAG_OPEN_STATE: tag_open_state(p, a); break;
        case END_TAG_OPEN_STATE: end_tag_open_state(p, a); break;
        case TAG_NAME_STATE: tag_name_state(p, a); break;
        case RCDATA_LESS_THAN_SIGN_STATE: rcdata_less_than_sign_state(p, a); break;
        case RCDATA_END_TAG_OPEN_STATE: rcdata_end_tag_open_state(p, a); break;
        case RCDATA_END_TAG_NAME_STATE: rcdata_end_tag_name_state(p, a); break;
        case RAWTEXT_LESS_THAN_SIGN_STATE: rawtext_less_than_sign_state(p, a); break;
        case RAWTEXT_END_TAG_OPEN_STATE: rawtext_end_tag_open_state(p, a); break;
        case RAWTEXT_END_TAG_NAME_STATE: rawtext_end_tag_name_state(p, a); break;
        case SCRIPT_DATA_LESS_THAN_SIGN_STATE: script_data_less_than_sign_state(p, a); break;
        case SCRIPT_DATA_END_TAG_OPEN_STATE: script_data_end_tag_open_state(p, a); break;
        case SCRIPT_DATA_END_TAG_NAME_STATE: script_data_end_tag_name_state(p, a); break;
        case SCRIPT_DATA_ESCAPE_START_STATE: script_data_escape_start_state(p, a); break;
        case SCRIPT_DATA_ESCAPE_START_DASH_STATE: script_data_escape_start_dash_state(p, a); break;
        case SCRIPT_DATA_ESCAPED_STATE: script_data_escaped_state(p, a); break;
        case SCRIPT_DATA_ESCAPED_DASH_STATE: script_data_escaped_dash_state(p, a); break;
        case SCRIPT_DATA_ESCAPED_DASH_DASH_STATE: script_data_escaped_dash_dash_state(p, a); break;
        case SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE: script_data_escaped_less_than_sign_state(p, a); break;
        case SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE: script_data_escaped_end_tag_open_state(p, a); break;
        case SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE: script_data_escaped_end_tag_name_state(p, a); break;
        case SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE: script_data_double_escape_start_state(p, a); break;
        case SCRIPT_DATA_DOUBLE_ESCAPED_STATE: script_data_double_escaped_state(p, a); break;
        case SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE: script_data_double_escaped_dash_state(p, a); break;
        case SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE: script_data_double_escaped_dash_dash_state(p, a); break;
        case SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE: script_data_double_escaped_less_than_sign_state(p, a); break;
        case SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE: script_data_double_escape_end_state(p, a); break;
        case BEFORE_ATTRIBUTE_NAME_STATE: before_attribute_name_state(p, a); break;
        case ATTRIBUTE_NAME_STATE: attribute_name_state(p, a); break;
        case AFTER_ATTRIBUTE_NAME_STATE: after_attribute_name_state(p, a); break;
        case BEFORE_ATTRIBUTE_VALUE_STATE: before_attribute_value_state(p, a); break;
        case ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE: attribute_value_double_quoted_state(p, a); break;
        case ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE: attribute_value_single_quoted_state(p, a); break;
        case ATTRIBUTE_VALUE_UNQUOTED_STATE: attribute_value_unquoted_state(p, a); break;
        case AFTER_ATTRIBUTE_VALUE_QUOTED_STATE: after_attribute_value_quoted_state(p, a); break;
        case SELF_CLOSING_START_TAG_STATE: self_closing_start_tag_state(p, a); break;
        case BOGUS_COMMENT_STATE: bogus_comment_state(p, a); break;
        case MARKUP_DECLARATION_OPEN_STATE: markup_declaration_open_state(p, a); break;
        case COMMENT_START_STATE: comment_start_state(p, a); break;
        case COMMENT_START_DASH_STATE: comment_start_dash_state(p, a); break;
        case COMMENT_STATE: comment_state(p, a); break;
        case COMMENT_LESS_THAN_SIGN_STATE: comment_less_than_sign_state(p, a); break;
        case COMMENT_LESS_THAN_SIGN_BANG_STATE: comment_less_than_sign_bang_state(p, a); break;
        case COMMENT_LESS_THAN_SIGN_BANG_DASH_STATE: comment_less_than_sign_bang_dash_state(p, a); break;
        case COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH_STATE: comment_less_than_sign_bang_dash_dash_state(p, a); break;
        case COMMENT_END_DASH_STATE: comment_end_dash_state(p, a); break;
        case COMMENT_END_STATE: comment_end_state(p, a); break;
        case COMMENT_END_BANG_STATE: comment_end_bang_state(p, a); break;
        case DOCTYPE_STATE: doctype_state(p, a); break;
        case BEFORE_DOCTYPE_NAME_STATE: before_doctype_name_state(p, a); break;
        case DOCTYPE_NAME_STATE: doctype_name_state(p, a); break;
        case AFTER_DOCTYPE_NAME_STATE: after_doctype_name_state(p, a); break;
        case AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE: after_doctype_public_keyword_state(p, a); break;
        case BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE: before_doctype_public_identifier_state(p, a); break;
        case DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE: doctype_public_identifier_double_quoted_state(p, a); break;
        case DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE: doctype_public_identifier_single_quoted_state(p, a); break;
        case AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE: after_doctype_public_identifier_state(p, a); break;
        case BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE: between_doctype_public_and_system_identifiers_state(p, a); break;
        case AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE: after_doctype_system_keyword_state(p, a); break;
        case BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE: before_doctype_system_identifier_state(p, a); break;
        case DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE: doctype_system_identifier_double_quoted_state(p, a); break;
        case DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE: doctype_system_identifier_single_quoted_state(p, a); break;
        case AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE: after_doctype_system_identifier_state(p, a); break;
        case BOGUS_DOCTYPE_STATE: bogus_doctype_state(p, a); break;
        case CDATA_SECTION_STATE: cdata_section_state(p, a); break;
        case CDATA_SECTION_BRACKET_STATE: cdata_section_bracket_state(p, a); break;
        case CDATA_SECTION_END_STATE: cdata_section_end_state(p, a); break;
        case CHARACTER_REFERENCE_STATE: character_reference_state(p, a); break;
        case NAMED_CHARACTER_REFERENCE_STATE: named_character_reference_state(p, a); break;
        case AMBIGUOUS_AMPERSAND_STATE: ambiguous_ampersand_state(p, a); break;
        case NUMERIC_CHARACTER_REFERENCE_STATE: numeric_character_reference_state(p, a); break;
        case HEXADECIMAL_CHARACTER_REFERENCE_START_STATE: hexadecimal_character_reference_start_state(p, a); break;
        case DECIMAL_CHARACTER_REFERENCE_START_STATE: decimal_character_reference_start_state(p, a); break;
        case HEXADECIMAL_CHARACTER_REFERENCE_STATE: hexadecimal_character_reference_state(p, a); break;
        case DECIMAL_CHARACTER_REFERENCE_STATE: decimal_character_reference_state(p, a); break;
        case NUMERIC_CHARACTER_REFERENCE_END_STATE: numeric_character_reference_end_state(p, a); break;
        default: LOG_ERROR(err_to_str(INVALID_TOKENIZER_STATE_ERROR));
    }
}

void data_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            character_token_emit(c); //logging
            emit_token(token_character_init(a, c));
            break;
        case EOF:
            eof_token_emit(); //logging
            emit_token(token_eof_init(a));
            break;
        default:
            character_token_emit(c); //logging
            emit_token(token_character_init(a, c));
    }
}

void rcdata_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            character_token_emit(UNICODE_REPLACEMENT_CHAR); 
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void rawtext_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '<':
            set_state(p, RAWTEXT_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            character_token_emit(UNICODE_REPLACEMENT_CHAR); 
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void script_data_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '<':
            set_state(p, SCRIPT_DATA_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            character_token_emit(UNICODE_REPLACEMENT_CHAR); 
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void plaintext_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            character_token_emit(UNICODE_REPLACEMENT_CHAR); 
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void tag_open_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '!':
            set_state(p, MARKUP_DECLARATION_OPEN_STATE);
            break;
        case '/':
            set_state(p, END_TAG_OPEN_STATE);
            break;
        case '?':
            LOG_ERROR(err_to_str(UNEXPECTED_QUESTION_MARK_INSTEAD_OF_TAG_NAME_PARSE_ERROR));
            set_current_token(p, token_comment_init(a));
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_COMMENT_STATE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_BEFORE_TAG_NAME_PARSE_ERROR));
            character_token_emit('<');
            emit_token(token_character_init(a, '<'));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            if (is_ascii_alpha(c)) {
                set_current_token(p, token_start_tag_init(a));
                input_system_reconsume(&p->input);
                set_state(p, TAG_NAME_STATE);
                return;
            } else {
                LOG_ERROR(err_to_str(INVALID_FIRST_CHARACTER_OF_TAG_NAME_PARSE_ERROR));
                character_token_emit('<');
                emit_token(token_character_init(a, '<'));
                input_system_reconsume(&p->input);
                set_state(p, DATA_STATE);
            }
    }
}

void end_tag_open_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '>':
            LOG_ERROR(err_to_str(MISSING_END_TAG_NAME_PARSE_ERROR));
            set_state(p, DATA_STATE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_BEFORE_TAG_NAME_PARSE_ERROR));
            character_token_emit('/');
            eof_token_emit();
            emit_token(token_character_init(a, '/'));
            emit_token(token_eof_init(a));
            break;
        default:
            if (is_ascii_alpha(c)) {
                set_current_token(p, token_end_tag_init(a));
                input_system_reconsume(&p->input);
                set_state(p, TAG_NAME_STATE);
            } else {
                LOG_ERROR(err_to_str(INVALID_FIRST_CHARACTER_OF_TAG_NAME_PARSE_ERROR));
                set_current_token(p, token_comment_init(a));
                input_system_reconsume(&p->input);
                set_state(p, BOGUS_COMMENT_STATE);
            }
    }
}

void tag_name_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_name(UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_TAG_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(tolower(c));
            } else {
                append_to_current_tag_token_name(c);
            }
    }
}

void rcdata_less_than_sign_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (c == '/') {
        clear_temporary_buffer(); //set temp buffer to ""
        set_state(p, RCDATA_END_TAG_OPEN_STATE);
    } else {
        character_token_emit('<');
        emit_token(token_character_init(a, '<'));
        input_system_reconsume(&p->input);
        set_state(p, RCDATA_STATE);
    }
}

void rcdata_end_tag_open_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alpha(c)) {
        set_current_token(p, token_eof_init(a));
        input_system_reconsume(&p->input);
        set_state(p, RCDATA_END_TAG_NAME_STATE);
    } else {
        character_token_emit('<');
        character_token_emit('/');
        emit_token(token_character_init(a, '<'));
        emit_token(token_character_init(a, '/'));
        input_system_reconsume(&p->input);
        set_state(p, RCDATA_STATE);
    }
}

void rcdata_end_tag_name_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, RCDATA_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, SELF_CLOSING_START_TAG_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, RCDATA_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, DATA_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, RCDATA_STATE);
            }
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(tolower(c));
                append_to_temp_buffer(c);
            } else if (is_ascii_lower_alpha(c)) {
                append_to_current_tag_token_name(c);
                append_to_temp_buffer(c);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, RCDATA_STATE);
            }
    }
}

void rawtext_less_than_sign_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (c == '/') {
        clear_temporary_buffer();
        set_state(p, RAWTEXT_END_TAG_OPEN_STATE);
    } else {
        character_token_emit('<');
        emit_token(token_character_init(a, '<'));
        input_system_reconsume(&p->input);
        set_state(p, RAWTEXT_STATE);
    }
}

void rawtext_end_tag_open_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alpha(c)) {
        set_current_token(p, token_eof_init(a));
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_END_TAG_NAME_STATE);
    } else {
        character_token_emit('/');
        emit_token(token_character_init(a, '/'));
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_STATE);
    }
}

void rawtext_end_tag_name_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\f':
        case '\n':
        case ' ':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, RAWTEXT_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, SELF_CLOSING_START_TAG_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, DATA_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, DATA_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, DATA_STATE);
            }
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(tolower(c));
                append_to_temp_buffer(c);
            } else if (is_ascii_lower_alpha(c)) {
                append_to_current_tag_token_name(c);
                append_to_temp_buffer(c);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, RAWTEXT_STATE);
            }
    }
}

void script_data_less_than_sign_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '/':
            clear_temporary_buffer();
            set_state(p, SCRIPT_DATA_END_TAG_OPEN_STATE);
            break;
        case '!':
            set_state(p, SCRIPT_DATA_ESCAPE_START_STATE);
            character_token_emit('<');
            character_token_emit('!');
            emit_token(token_character_init(a, '<'));
            emit_token(token_character_init(a, '!'));
            break;
        default:
            character_token_emit('<');
            emit_token(token_character_init(a, '<'));
            set_state(p, SCRIPT_DATA_STATE);
    }
}

void script_data_end_tag_open_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alpha(c)) {
        set_current_token(p, token_end_tag_init(a));
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_END_TAG_NAME_STATE);
    } else {
        character_token_emit('<');
        character_token_emit('/');
        emit_token(token_character_init(a, '<'));
        emit_token(token_character_init(a, '/'));
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_STATE);
    }
}

void script_data_end_tag_name_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, SELF_CLOSING_START_TAG_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, DATA_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_STATE);
            }
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(tolower(c));
                append_to_temp_buffer(c);
            } else if (is_ascii_lower_alpha(c)) {
                append_to_current_tag_token_name(c);
                append_to_temp_buffer(c);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_STATE);
            }
    }
}

void script_data_escape_start_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (c == '-') {
        set_state(p, SCRIPT_DATA_ESCAPE_START_DASH_STATE);
        character_token_emit('-');
        emit_token(token_character_init(a, '-'));
    } else {
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_STATE);
    }
}

void script_data_escape_start_dash_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (c == '-') {
        set_state(p, SCRIPT_DATA_ESCAPED_DASH_DASH_STATE);
        character_token_emit('-');
        emit_token(token_character_init(a, '-'));
    } else {
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_STATE);
    }
}

void script_data_escaped_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, SCRIPT_DATA_ESCAPED_DASH_STATE);
            character_token_emit('-');
            emit_token(token_character_init(a, '-'));
            break;
        case '<':
            set_state(p, SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            character_token_emit(UNICODE_REPLACEMENT_CHAR);
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF: 
            LOG_ERROR(err_to_str(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void script_data_escaped_dash_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-': 
            set_state(p, SCRIPT_DATA_ESCAPED_DASH_DASH_STATE);
            break;
        case '<': 
            set_state(p, SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            character_token_emit(UNICODE_REPLACEMENT_CHAR);
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF: 
            LOG_ERROR(err_to_str(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void script_data_escaped_dash_dash_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-': 
            character_token_emit('-');
            emit_token(token_character_init(a, '-'));
            break;
        case '<': 
            set_state(p, SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE);
            break;
        case '>': 
            set_state(p, SCRIPT_DATA_STATE);
            character_token_emit('>');
            emit_token(token_character_init(a, '>'));
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            character_token_emit(UNICODE_REPLACEMENT_CHAR);
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF: 
            LOG_ERROR(err_to_str(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void script_data_escaped_less_than_sign_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '/':
            clear_temporary_buffer();
            set_state(p, SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE);
            break;
        default:
            if (is_ascii_alpha(c)) {
                clear_temporary_buffer();
                character_token_emit('<');
                emit_token(token_character_init(a, '<'));
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE);
            } else {
                character_token_emit('<');
                emit_token(token_character_init(a, '<'));
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
    }
}

void script_data_escaped_end_tag_open_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alpha(c)) {
        set_current_token(p, token_end_tag_init(a));
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE);
    } else {
        character_token_emit('<');
        character_token_emit('/');
        emit_token(token_character_init(a, '<'));
        emit_token(token_character_init(a, '/'));
        input_system_reconsume(&p->input);
        set_state(p, SCRIPT_DATA_ESCAPED_STATE);
    }
}

void script_data_escaped_end_tag_name_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, SELF_CLOSING_START_TAG_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(p, DATA_STATE);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(tolower(c));
                append_to_temp_buffer(c);
            } else if (is_ascii_lower_alpha(c)) {
                append_to_current_tag_token_name(c);
                append_to_temp_buffer(c);
            } else {
                character_token_emit('<');
                character_token_emit('/');
                emit_token(token_character_init(a, '<'));
                emit_token(token_character_init(a, '/'));
                emit_tokens_in_temp_buffer(); 
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
    }
}

void script_data_double_escape_start_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
            if (strncmp(get_temporary_buffer(), "script", 6) == 0) {
                set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            } else {
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
            character_token_emit(c);
            emit_token(token_character_init(a, c));
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_temp_buffer(tolower(c));
                character_token_emit(c);
                emit_token(token_character_init(a, c));
            } else if (is_ascii_lower_alpha(c)) {
                append_to_temp_buffer(c);
                character_token_emit(c);
                emit_token(token_character_init(a, c));
            } else {
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            }
    }
}

void script_data_double_escaped_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '-':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE);
            character_token_emit('-');
            emit_token(token_character_init(a, '-'));
            break;
        case '<':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE);
            character_token_emit('<');
            emit_token(token_character_init(a, '<'));
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            character_token_emit(UNICODE_REPLACEMENT_CHAR);
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void script_data_double_escaped_dash_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch(c) {
        case '-':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE);
            character_token_emit('-');
            emit_token(token_character_init(a, '-'));
            break;
        case '<':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE);
            character_token_emit('<');
            emit_token(token_character_init(a, '<'));
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            character_token_emit(UNICODE_REPLACEMENT_CHAR);
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void script_data_double_escaped_dash_dash_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            character_token_emit('-');
            emit_token(token_character_init(a, '-'));
            break;
        case '<':
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE);
            character_token_emit('<');
            emit_token(token_character_init(a, '<'));
            break;
        case '>':
            set_state(p, SCRIPT_DATA_STATE);
            character_token_emit('>');
            emit_token(token_character_init(a, '>'));
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            character_token_emit(UNICODE_REPLACEMENT_CHAR);
            emit_token(token_character_init(a, UNICODE_REPLACEMENT_CHAR));
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void script_data_double_escaped_less_than_sign_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '/':
            clear_temporary_buffer();
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE);
            character_token_emit('/');
            emit_token(token_character_init(a, '/'));
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
    }
}

void script_data_double_escape_end_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
            if (strncmp(get_temporary_buffer(), "script", 6) == 0) {
                set_state(p, SCRIPT_DATA_ESCAPED_STATE);
            } else {
                set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            }
            character_token_emit(c);
            emit_token(token_character_init(a, c));
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_temp_buffer(tolower(c));
                character_token_emit(c);
                emit_token(token_character_init(a, c));
            } else if (is_ascii_lower_alpha(c)) {
                append_to_temp_buffer(c);
                character_token_emit(c);
                emit_token(token_character_init(a, c));
            } else {
                input_system_reconsume(&p->input);
                set_state(p, SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            }
    }
}

void before_attribute_name_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(UNEXPECTED_EQUALS_SIGN_BEFORE_ATTRIBUTE_NAME_PARSE_ERROR));
            start_new_attribute_for_current_tag_token();
            append_to_current_tag_token_attribute_name(c);
            set_state(p, ATTRIBUTE_NAME_STATE);
            break;
        default:
            start_new_attribute_for_current_tag_token();
            input_system_reconsume(&p->input);
            set_state(p, ATTRIBUTE_NAME_STATE);
    }
}

void attribute_name_state(parser * p, arena * a) {
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
            check_for_duplicate_attributes();
            break;
        case '=':
            set_state(p, BEFORE_ATTRIBUTE_VALUE_STATE);
            check_for_duplicate_attributes();
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_attribute_name(UNICODE_REPLACEMENT_CHAR);
            break;
        case '"':
        case '\'':
        case '<':
            LOG_ERROR(err_to_str(UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR));
            append_to_current_tag_token_attribute_name(c);
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_attribute_name(tolower(c));
            } else {
                append_to_current_tag_token_attribute_name(c);
            }
    }
}

void after_attribute_name_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(EOF_IN_TAG_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            start_new_attribute_for_current_tag_token();
            input_system_reconsume(&p->input);
            set_state(p, ATTRIBUTE_NAME_STATE);
    }
}

void before_attribute_value_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(MISSING_ATTRIBUTE_VALUE_PARSE_ERROR));
            set_state(p, DATA_STATE);
            emit_current_token(p, TOKEN_TAG);
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, ATTRIBUTE_VALUE_UNQUOTED_STATE);
    }
}

void attribute_value_double_quoted_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_TAG_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_attribute_value(c);
    }
}

void attribute_value_single_quoted_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_TAG_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_attribute_value(c);
    }
}

void attribute_value_unquoted_state(parser * p, arena * a) {
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
            emit_current_token(p, TOKEN_TAG);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(UNICODE_REPLACEMENT_CHAR);
            break;
        case '"':
        case '\'':
        case '<':
        case '=':
        case '`':
            LOG_ERROR(err_to_str(UNEXPECTED_CHARACTER_IN_UNQUOTED_ATTRIBUTE_VALUE_PARSE_ERROR));
            append_to_current_tag_token_attribute_value(c);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_TAG_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_attribute_value(c);
    }
}

void after_attribute_value_quoted_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(EOF_IN_TAG_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            LOG_ERROR(err_to_str(MISSING_WHITESPACE_BETWEEN_ATTRIBUTES_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
    }
}

void self_closing_start_tag_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            set_self_closing_tag_for_current_token(true);
            set_state(p, DATA_STATE);
            emit_current_token(p, TOKEN_TAG);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_TAG_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            LOG_ERROR(err_to_str(UNEXPECTED_SOLIDUS_IN_TAG_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, BEFORE_ATTRIBUTE_NAME_STATE);
    }
}

void bogus_comment_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            set_state(p, DATA_STATE);
            emit_current_token(p, COMMENT); //should be a comment token
            break;
        case EOF:
            emit_current_token(p, COMMENT);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_comment_data(UNICODE_REPLACEMENT_CHAR);
            break;
        default:
            append_to_current_tag_token_comment_data(c);
    }
}

void markup_declaration_open_state(parser * p, arena * a) {
    string buf = input_system_peekn(&p->input, 7, *a); //TODO: use arena

    if (buf.data[0] == '-' && buf.data[1] == '-') {
        input_system_consume(&p->input);
        input_system_consume(&p->input);
        set_current_token(p, token_comment_init(a));
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
                LOG_ERROR(err_to_str(CDATA_IN_HTML_CONTENT_PARSE_ERROR));
                set_current_token(p, token_comment_init(a));
                append_to_current_tag_token_comment_data('[');
                append_to_current_tag_token_comment_data('C');
                append_to_current_tag_token_comment_data('D');
                append_to_current_tag_token_comment_data('A');
                append_to_current_tag_token_comment_data('T');
                append_to_current_tag_token_comment_data('A');
                append_to_current_tag_token_comment_data('[');
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

    LOG_ERROR(err_to_str(INCORRECTLY_OPENED_COMMENT_PARSE_ERROR));
    set_current_token(p, token_comment_init(a));
    set_state(p, BOGUS_COMMENT_STATE);
}

void comment_start_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, COMMENT_START_DASH_STATE);
            break;
        case '>':
            LOG_ERROR(err_to_str(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR));
            set_state(p, DATA_STATE);
            emit_current_token(p, COMMENT);
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_start_dash_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, COMMENT_END_STATE);
            break;
        case '>':
            LOG_ERROR(err_to_str(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR));
            set_state(p, DATA_STATE);
            emit_current_token(p, COMMENT);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_COMMENT_PARSE_ERROR));
            emit_current_token(p, COMMENT);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_comment_data('-');
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '<':
            append_to_current_tag_token_comment_data(c);
            set_state(p, COMMENT_LESS_THAN_SIGN_STATE);
            break;
        case '-':
            set_state(p, COMMENT_END_DASH_STATE);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_comment_data(UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_COMMENT_PARSE_ERROR));
            emit_current_token(p, COMMENT);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_comment_data(c);
    }
}

void comment_less_than_sign_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '!':
            append_to_current_tag_token_comment_data(c);
            set_state(p, COMMENT_LESS_THAN_SIGN_BANG_STATE);
            break;
        case '<':
            append_to_current_tag_token_comment_data(c);
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_less_than_sign_bang_state(parser * p, arena * a) {
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

void comment_less_than_sign_bang_dash_state(parser * p, arena * a) {
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

void comment_less_than_sign_bang_dash_dash_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
        case EOF:
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_END_STATE);
            break;
        default:
            LOG_ERROR(err_to_str(NESTED_COMMENT_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_END_STATE);
    }
}

void comment_end_dash_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            set_state(p, COMMENT_END_STATE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_COMMENT_PARSE_ERROR));
            emit_current_token(p, COMMENT);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_comment_data('-');
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_end_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            set_state(p, DATA_STATE);
            emit_current_token(p, COMMENT);
            break;
        case '!':
            set_state(p, COMMENT_END_BANG_STATE);
            break;
        case '-':
            append_to_current_tag_token_comment_data('-');
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_COMMENT_PARSE_ERROR));
            emit_current_token(p, COMMENT);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('-');
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void comment_end_bang_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '-':
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('!');
            set_state(p, COMMENT_END_DASH_STATE);
            break;
        case '>':
            LOG_ERROR(err_to_str(INCORRECTLY_CLOSED_COMMENT_PARSE_ERROR));
            set_state(p, DATA_STATE);
            emit_current_token(p, COMMENT);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_COMMENT_PARSE_ERROR));
            emit_current_token(p, COMMENT);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('!');
            input_system_reconsume(&p->input);
            set_state(p, COMMENT_STATE);
    }
}

void doctype_state(parser * p, arena * a) {
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
            LOG_ERROR(err_to_str(EOF_IN_COMMENT_PARSE_ERROR));
            set_current_token(p, token_doctype_init(a));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, TOKEN_TYPE_COUNT);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            LOG_ERROR(err_to_str(MISSING_WHITESPACE_BEFORE_DOCTYPE_NAME_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, BEFORE_DOCTYPE_NAME_STATE);
    }
}

void before_doctype_name_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            // intentionally ignore character
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            set_current_token(p, token_doctype_init(a));
            append_to_current_tag_token_name(UNICODE_REPLACEMENT_CHAR);
            set_state(p, DOCTYPE_NAME_STATE);
            break;
        case '>':
            LOG_ERROR(err_to_str(MISSING_DOCTYPE_NAME_PARSE_ERROR));
            set_current_token(p, token_doctype_init(a));
            set_doctype_token_force_quirks_flag(true);
            set_state(p, DATA_STATE);
            emit_current_token(p, TOKEN_TYPE_COUNT);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_current_token(p, token_doctype_init(a));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, TOKEN_TYPE_COUNT);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                set_current_token(p, token_doctype_init(a));
                append_to_current_tag_token_name(tolower(c));
                set_state(p, DOCTYPE_NAME_STATE);
            } else {
                set_current_token(p, token_doctype_init(a));
                append_to_current_tag_token_name(c);
                set_state(p, DOCTYPE_NAME_STATE);
            }
    }
}

void doctype_name_state(parser * p, arena * a) {
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
            emit_current_token(p, DOCTYPE);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_name(UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            if (is_ascii_upper_alpha(c)) {
                append_to_current_tag_token_name(tolower(c));
            } else {
                append_to_current_tag_token_name(c);
            }
    }
}

void after_doctype_name_state(parser * p, arena * a) {
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
            emit_current_token(p, DOCTYPE);
            return;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            return;
    }

    string temp = input_system_peekn(&p->input, 5, *a);
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

    LOG_ERROR(err_to_str(INVALID_CHARACTER_SEQUENCE_AFTER_DOCTYPE_NAME_PARSE_ERROR));
    set_doctype_token_force_quirks_flag(true);
    input_system_reconsume(&p->input);
    set_state(p, BOGUS_DOCTYPE_STATE);
}

void after_doctype_public_keyword_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE);
            break;
        case '"':
            LOG_ERROR(err_to_str(MISSING_WHITESPACE_AFTER_DOCTYPE_PUBLIC_KEYWORD_PARSE_ERROR));
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            LOG_ERROR(err_to_str(MISSING_WHITESPACE_AFTER_DOCTYPE_PUBLIC_KEYWORD_PARSE_ERROR));
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            LOG_ERROR(err_to_str(MISSING_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            set_state(p, DATA_STATE);
            emit_current_token(p, DOCTYPE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            LOG_ERROR(err_to_str(MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void before_doctype_public_identifier_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore character
            break;
        case '"':
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            LOG_ERROR(err_to_str(MISSING_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            set_state(p, DATA_STATE);
            emit_current_token(p, DOCTYPE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            LOG_ERROR(err_to_str(MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void doctype_public_identifier_double_quoted_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '"':
            set_state(p, AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE);
            break;
        case '>':
            LOG_ERROR(err_to_str(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            set_state(p, DATA_STATE);
            emit_current_token(p, DOCTYPE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            break;
        default:
            append_to_current_tag_token_identifier(c);
    }
}

void doctype_public_identifier_single_quoted_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '"':
            set_state(p, AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_identifier(UNICODE_REPLACEMENT_CHAR);
            break;
        case '>':
            LOG_ERROR(err_to_str(ABRUPT_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            set_state(p, DATA_STATE);
            emit_current_token(p, DOCTYPE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_identifier(c);
    }
}

void after_doctype_public_identifier_state(parser * p, arena * a) {
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
            emit_current_token(p, DOCTYPE);
            break;
        case '"':
            LOG_ERROR(err_to_str(MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_PARSE_ERROR));
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            LOG_ERROR(err_to_str(MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_PARSE_ERROR));
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            LOG_ERROR(err_to_str(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void between_doctype_public_and_system_identifiers_state(parser * p, arena * a) {
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
            emit_current_token(p, DOCTYPE);
            break;
        case '"':
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            LOG_ERROR(err_to_str(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void after_doctype_system_keyword_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(p, BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE);
            break;
        case '"':
            LOG_ERROR(err_to_str(MISSING_WHITESPACE_AFTER_DOCTYPE_SYSTEM_KEYWORD_PARSE_ERROR));
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            LOG_ERROR(err_to_str(MISSING_WHITESPACE_AFTER_DOCTYPE_SYSTEM_KEYWORD_PARSE_ERROR));
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            LOG_ERROR(err_to_str(MISSING_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            LOG_ERROR(err_to_str(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void before_doctype_system_identifier_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore character
            break;
        case '"':
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_current_token_identifier("", 0);
            set_state(p, DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            LOG_ERROR(err_to_str(MISSING_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            set_state(p, DATA_STATE);
            emit_current_token(p, DOCTYPE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            LOG_ERROR(err_to_str(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
            break;
    }
}

void doctype_system_identifier_double_quoted_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '"':
            set_state(p, AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_identifier(UNICODE_REPLACEMENT_CHAR);
            break;
        case '>':
            LOG_ERROR(err_to_str(ABRUPT_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            set_state(p, DATA_STATE);
            emit_current_token(p, DOCTYPE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_identifier(c);
    }
}

void doctype_system_identifier_single_quoted_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '\'':
            set_state(p, AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE);
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            append_to_current_tag_token_identifier(UNICODE_REPLACEMENT_CHAR);
            break;
        case '>':
            LOG_ERROR(err_to_str(ABRUPT_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            set_state(p, DATA_STATE);
            emit_current_token(p, DOCTYPE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_DOCTYPE_PARSE_ERROR));
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(p, DOCTYPE);
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            append_to_current_tag_token_identifier(c);
    }
}

void after_doctype_system_identifier_state(parser * p, arena * a) {
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
            emit_current_token(p, DOCTYPE);
            break;
        default:
            LOG_ERROR(err_to_str(UNEXPECTED_CHARACTER_AFTER_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR));
            input_system_reconsume(&p->input);
            set_state(p, BOGUS_DOCTYPE_STATE);
    }
}

void bogus_doctype_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case '>':
            set_state(p, DATA_STATE);
            emit_token(token_doctype_init(a));
            break;
        case '\0':
            LOG_ERROR(err_to_str(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR));
            // intentionally ignore character
            break;
        case EOF:
            eof_token_emit(); //logging
            emit_current_token(p, DOCTYPE);
            emit_token(token_eof_init(a));
            break;
            // otherwise intentionally ignore character
    }
}

void cdata_section_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case ']':
            set_state(p, CDATA_SECTION_BRACKET_STATE);
            break;
        case EOF:
            LOG_ERROR(err_to_str(EOF_IN_CDATA_PARSE_ERROR));
            eof_token_emit();
            emit_token(token_eof_init(a));
            break;
        default:
            character_token_emit(c);
            emit_token(token_character_init(a, c));
    }
}

void cdata_section_bracket_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case ']':
            set_state(p, CDATA_SECTION_END_STATE);
            break;
        default:
            character_token_emit(']');
            emit_token(token_character_init(a, ']'));
            input_system_reconsume(&p->input);
            set_state(p, CDATA_SECTION_STATE);
    }
}

void cdata_section_end_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    switch (c) {
        case ']':
            character_token_emit(']');
            emit_token(token_character_init(a, ']'));
            break;
        case '>':
            set_state(p, DATA_STATE);
            break;
        default:
            character_token_emit(']');
            character_token_emit(']');
            emit_token(token_character_init(a, ']'));
            emit_token(token_character_init(a, ']'));
            input_system_reconsume(&p->input);
            set_state(p, CDATA_SECTION_STATE);
    }
}

void character_reference_state(parser * p, arena * a) {
    clear_temporary_buffer();
    append_to_temp_buffer('&');
    int c = input_system_consume(&p->input);
    switch (c) {
        case '#':
            append_to_temp_buffer(c);
            set_state(p, NUMERIC_CHARACTER_REFERENCE_STATE);
            break;
        default:
            if (is_ascii_alphanumeric(c)) {
                input_system_reconsume(&p->input);
                set_state(p, NAMED_CHARACTER_REFERENCE_STATE);
            } else {
                flush_code_points();
                input_system_reconsume(&p->input);
                return_state(p);
            }
    }
}

//TODO: double check the logic of this one 
void named_character_reference_state(parser * p, arena * a) {
    while (is_named_character(input_system_peek(&p->input))) {
        int c = input_system_consume(&p->input);
        append_to_temp_buffer(c);

        if (is_part_of_an_attribute() 
                && c != ';' 
                && !isalnum(c)
                && (input_system_peek(&p->input) == '=' || isalnum(input_system_peek(&p->input)) )) {
            flush_code_points();
            return_state(p);
            return;
        } else {
            if (c != ';') {
                LOG_ERROR(err_to_str(MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE_PARSE_ERROR));
            }
            clear_temporary_buffer();
            interpret_character_reference_name();
            flush_code_points();
            return_state(p);
            return;
        }
    }
    flush_code_points();
    set_state(p, AMBIGUOUS_AMPERSAND_STATE);
}

void ambiguous_ampersand_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (is_ascii_alphanumeric(c)) {
        if (is_part_of_an_attribute()) {
            append_to_current_tag_token_attribute_value(c);
        } else {
            character_token_emit(c);
            emit_token(token_character_init(a, c));
        }
    } else if (c == ';') {
        LOG_ERROR(err_to_str(UNKNOWN_NAMED_CHARACTER_REFERENCE_PARSE_ERROR));
        input_system_reconsume(&p->input);
        return_state(p);
    } else {
        input_system_reconsume(&p->input);
        return_state(p);
    }
}

void numeric_character_reference_state(parser * p, arena * a) {
    set_character_reference_code(0);
    int c = input_system_consume(&p->input);
    switch (c) {
        case 'x':
        case 'X':
            append_to_temp_buffer(c);
            set_state(p, HEXADECIMAL_CHARACTER_REFERENCE_START_STATE);
            break;
        default:
            input_system_reconsume(&p->input);
            set_state(p, DECIMAL_CHARACTER_REFERENCE_START_STATE);
    }
}

void hexadecimal_character_reference_start_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (is_ascii_hex_digit(c)) {
        input_system_reconsume(&p->input);
        set_state(p, HEXADECIMAL_CHARACTER_REFERENCE_START_STATE);
    } else {
        LOG_ERROR(err_to_str(ABSENCE_OF_DIGITS_IN_NUMERIC_CHARACTER_REFERENCE_PARSE_ERROR));
        flush_code_points();
        input_system_reconsume(&p->input);
        return_state(p);
    }
}

void decimal_character_reference_start_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (is_ascii_digit(c)) {
        input_system_reconsume(&p->input);
        set_state(p, DECIMAL_CHARACTER_REFERENCE_STATE);
    } else {
        LOG_ERROR(err_to_str(ABSENCE_OF_DIGITS_IN_NUMERIC_CHARACTER_REFERENCE_PARSE_ERROR));
        flush_code_points();
        input_system_reconsume(&p->input);
        return_state(p);
    }
}

void hexadecimal_character_reference_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (is_ascii_digit(c)) {
        int curr_as_numeric = c - 0x0030;
        int ref_code = get_character_reference_code() * 16;
        ref_code += curr_as_numeric;
        set_character_reference_code(ref_code);
    } else if (is_ascii_upper_hex_digit(c)) {
        int curr_as_numeric = c - 0x0037;
        int ref_code = get_character_reference_code() * 16;
        ref_code += curr_as_numeric;
        set_character_reference_code(ref_code);
    } else if (is_ascii_lower_hex_digit(c)) {
        int curr_as_numeric = c - 0x0057;
        int ref_code = get_character_reference_code() * 16;
        ref_code += curr_as_numeric;
        set_character_reference_code(ref_code);
    } else if (c == ';') {
        set_state(p, NUMERIC_CHARACTER_REFERENCE_STATE);
    } else {
        LOG_ERROR(err_to_str(MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE_PARSE_ERROR));
        input_system_reconsume(&p->input);
        set_state(p, NUMERIC_CHARACTER_REFERENCE_END_STATE);
    }
}


void decimal_character_reference_state(parser * p, arena * a) {
    int c = input_system_consume(&p->input);
    if (is_ascii_digit(c)) {
        int curr_as_numeric = c - 0x0030;
        int ref_code = get_character_reference_code() * 10;
        ref_code += curr_as_numeric;
        set_character_reference_code(ref_code);
    } else if (c == ';') {
        set_state(p, NUMERIC_CHARACTER_REFERENCE_END_STATE);
    } else {
        LOG_ERROR(err_to_str(MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE_PARSE_ERROR));
        input_system_reconsume(&p->input);
        set_state(p, NUMERIC_CHARACTER_REFERENCE_END_STATE);
    }
}

void numeric_character_reference_end_state(parser * p, arena * a) {
    int char_ref = get_character_reference_code();
    if (char_ref == 0x00) {
        LOG_ERROR(err_to_str(NULL_CHARACTER_REFERENCE_PARSE_ERROR));
        set_character_reference_code(0xFFFD);
    } else if (char_ref > 0x10FFFF) {
        LOG_ERROR(err_to_str(CHARACTER_REFERENCE_OUTSIDE_OF_UNICODE_RANGE_PARSE_ERROR));
        set_character_reference_code(0xFFFD);
    } else if (is_surrogate(char_ref)) {
        LOG_ERROR(err_to_str(SURROGATE_CHARACTER_REFERENCE_PARSE_ERROR));
        set_character_reference_code(0xFFFD);
    } else if (is_non_char(char_ref)) {
        LOG_ERROR(err_to_str(NONCHARACTER_CHARACTER_REFERENCE_PARSE_ERROR));
    } else if (char_ref == 0x0D 
            || (is_control(char_ref) && !is_ascii_whitespace(char_ref))) {
        LOG_ERROR(err_to_str(CONTROL_CHARACTER_REFERENCE_PARSE_ERROR));
    } else {
#define map_case(num, code_point) case num: set_character_reference_code(code_point); break
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
        clear_temporary_buffer();
        append_to_temp_buffer(char_ref);
        flush_code_points();
        return_state(p);
    }
}
