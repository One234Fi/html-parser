#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

#include "lexer.h"
#include <ctype.h>

//state handlers
void data_state(lexer * lexer);
void rcdata_state(lexer * lexer);
void rawtext_state(lexer * lexer);
void script_data_state(lexer * lexer);
void plaintext_state(lexer * lexer);
void tag_open_state(lexer * lexer);
void end_tag_open_state(lexer * lexer);
void tag_name_state(lexer * lexer);
void rcdata_less_than_sign_state(lexer * lexer);
void rcdata_end_tag_open_state(lexer * lexer);
void rcdata_end_tag_name_state(lexer * lexer);
void rawtext_less_than_sign_state(lexer * lexer);
void rawtext_end_tag_open_state(lexer * lexer);
void rawtext_end_tag_name_state(lexer * lexer);
void script_data_less_than_sign_state(lexer * lexer);
void script_data_end_tag_open_state(lexer * lexer);
void script_data_end_tag_name_state(lexer * lexer);
void script_data_escape_start_state(lexer * lexer);
void script_data_escape_start_dash_state(lexer * lexer);
void script_data_escaped_state(lexer * lexer);
void script_data_escaped_dash_state(lexer * lexer);
void script_data_escaped_dash_dash_state(lexer * lexer);
void script_data_escaped_less_than_sign_state(lexer * lexer);
void script_data_escaped_end_tag_open_state(lexer * lexer);
void script_data_escaped_end_tag_name_state(lexer * lexer);
void script_data_double_escape_start_state(lexer * lexer);
void script_data_double_escaped_state(lexer * lexer);
void script_data_double_escaped_dash_state(lexer * lexer);
void script_data_double_escaped_dash_dash_state(lexer * lexer);
void script_data_double_escaped_less_than_sign_state(lexer * lexer);
void script_data_double_escape_end_state(lexer * lexer);
void before_attribute_name_state(lexer * lexer);
void attribute_name_state(lexer * lexer);
void after_attribute_name_state(lexer * lexer);
void before_attribute_value_state(lexer * lexer);
void attribute_value_double_quoted_state(lexer * lexer);
void attribute_value_single_quoted_state(lexer * lexer);
void attribute_value_unquoted_state(lexer * lexer);
void after_attribute_value_quoted_state(lexer * lexer);
void self_closing_start_tag_state(lexer * lexer);
void bogus_comment_state(lexer * lexer);
void markup_declaration_open_state(lexer * lexer);
void comment_start_state(lexer * lexer);
void comment_start_dash_state(lexer * lexer);
void comment_state(lexer * lexer);
void comment_less_than_sign_state(lexer * lexer);
void comment_less_than_sign_bang_state(lexer * lexer);
void comment_less_than_sign_bang_dash_state(lexer * lexer);
void comment_less_than_sign_bang_dash_dash_state(lexer * lexer);
void comment_end_dash_state(lexer * lexer);
void comment_end_state(lexer * lexer);
void comment_end_bang_state(lexer * lexer);
void doctype_state(lexer * lexer);
void before_doctype_name_state(lexer * lexer);
void doctype_name_state(lexer * lexer);
void after_doctype_name_state(lexer * lexer);
void after_doctype_public_keyword_state(lexer * lexer);
void before_doctype_public_identifier_state(lexer * lexer);
void doctype_public_identifier_double_quoted_state(lexer * lexer);
void doctype_public_identifier_single_quoted_state(lexer * lexer);
void after_doctype_public_identifier_state(lexer * lexer);
void between_doctype_public_and_system_identifiers_state(lexer * lexer);
void after_doctype_system_keyword_state(lexer * lexer);
void before_doctype_system_identifier_state(lexer * lexer);
void doctype_system_identifier_double_quoted_state(lexer * lexer);
void doctype_system_identifier_single_quoted_state(lexer * lexer);
void after_doctype_system_identifier_state(lexer * lexer);
void bogus_doctype_state(lexer * lexer);
void cdata_section_state(lexer * lexer);
void cdata_section_bracket_state(lexer * lexer);
void cdata_section_end_state(lexer * lexer);
void character_reference_state(lexer * lexer);
void named_character_reference_state(lexer * lexer);
void ambiguous_ampersand_state(lexer * lexer);
void numeric_character_reference_state(lexer * lexer);
void hexadecimal_character_reference_start_state(lexer * lexer);
void decimal_character_reference_start_state(lexer * lexer);
void hexadecimal_character_reference_state(lexer * lexer);
void decimal_character_reference_state(lexer * lexer);
void numeric_character_reference_end_state(lexer * lexer);
void invalid_state(lexer * lexer);

#define UNICODE_REPLACEMENT_CHAR 0xEF
void emit_token(lexer * lexer, token t);
void clear_temporary_buffer(lexer * lexer);
void append_to_temp_buffer(lexer * lexer, int c);
void emit_tokens_in_temp_buffer(lexer * lexer);
const char* get_temporary_buffer(lexer * lexer);
bool is_part_of_an_attribute(lexer * lexer);
void flush_code_points(lexer * lexer);
bool current_token_is_valid(lexer * lexer);
void set_current_token_identifier(lexer * lexer, const char * val, size len);
void append_to_current_tag_token_comment_data(lexer * lexer, int c);
void append_to_current_tag_token_identifier(lexer * lexer, int c);
void append_to_current_tag_token_attribute_value(lexer * lexer, int c);

void set_doctype_token_force_quirks_flag(lexer * lexer, bool b);
void set_self_closing_tag_for_current_token(lexer * lexer, bool b);
void append_to_current_tag_token_name(lexer * lexer, int c);
void start_new_attribute_for_current_tag_token(lexer * lexer);
void append_to_current_tag_token_attribute_name(lexer * lexer, int c);
void append_to_current_tag_token_attribute_value(lexer * lexer, int c);

char input_system_peek(input_system * s);
string input_system_peekn(input_system * s, int n, arena * a);
void input_system_reconsume(input_system * s);
int input_system_consume(input_system * s);
input_system input_system_init(const char* filename, arena * a);
bool input_system_empty(input_system * s);

void set_current_token(lexer * p, token tkn);
token get_current_token(lexer * p);

token token_start_tag_init();
token token_end_tag_init();
token token_comment_init();
token token_character_init(char c);
token token_eof_init();
token token_doctype_init();
token token_init(token_type type);

/**
 * Code point utils and definitions
 *
 * Code points (unicode characters) follow the format:
 *      U+XXXX
 *  Where U+ is the unicode prefix and 'X' is a hexadecimal digit
 *  
 *  In this project, unicode character literals use 0x as their prefix.
 *  U+ is also a spec-conformat prefix that may be seen in strings 
 *  and comments
 */

#define is_leading_surrogate(c) (0xD800 <= c && c <= 0xDBFF)
#define is_trailing_surrogate(c) (0xDC00 <= c && c <= 0xDFFF)
#define is_surrogate(c) (is_leading_surrogate(c) || is_trailing_surrogate(c))
#define is_scalar_value(c) (!is_surrogate(c))
#define is_non_char(c) (                \
        (0xFDD0 <= c && c <= 0xFDEF)    \
        || c == 0xFFFE                  \
        || c == 0xFFFF                  \
        || c == 0x1FFFE                 \
        || c == 0x1FFFF                 \
        || c == 0x2FFFE                 \
        || c == 0x2FFFF                 \
        || c == 0x3FFFE                 \
        || c == 0x3FFFF                 \
        || c == 0x4FFFE                 \
        || c == 0x4FFFF                 \
        || c == 0x5FFFE                 \
        || c == 0x5FFFF                 \
        || c == 0x6FFFE                 \
        || c == 0x6FFFF                 \
        || c == 0x7FFFE                 \
        || c == 0x7FFFF                 \
        || c == 0x8FFFE                 \
        || c == 0x8FFFF                 \
        || c == 0x9FFFE                 \
        || c == 0x9FFFF                 \
        || c == 0xAFFFE                 \
        || c == 0xAFFFF                 \
        || c == 0xBFFFE                 \
        || c == 0xBFFFF                 \
        || c == 0xCFFFE                 \
        || c == 0xCFFFF                 \
        || c == 0xDFFFE                 \
        || c == 0xDFFFF                 \
        || c == 0xEFFFE                 \
        || c == 0xEFFFF                 \
        || c == 0xFFFFE                 \
        || c == 0xFFFFF                 \
        || c == 0x10FFFE                \
        || c == 0x10FFFF                \
        )
#define is_ascii_code_point(c) (0x0000 <= c && c <= 0x007F)
#define is_ascii_tab_or_newline(c) (c == 0x0009 || c == 0x000A || c == 0x000D)
#define is_ascii_whitespace(c) (is_ascii_tab_or_newline(c) || c == 0x000C || c == 0x0020)
#define is_czero_ctrl(c) (c >= 0x0000 && c <= 0x001F) 
#define is_czero_ctrl_or_space(c) (is_czero_ctrl(c) || c == 0x0020)
#define is_control(c) (is_czero_ctrl(c) || (0x007F <= c && c <= 0x009F))
#define is_ascii_digit(c) (0x0030 <= c && c <= 0x0039)
#define is_ascii_upper_hex_digit(c) (is_ascii_digit(c) || (0x0041 <= c && c <= 0x0046))
#define is_ascii_lower_hex_digit(c) (is_ascii_digit(c) || (0x0061 <= c && c <= 0x0066))
#define is_ascii_hex_digit(c) (is_ascii_upper_hex_digit(c) || is_ascii_lower_hex_digit(c))
#define is_ascii_upper_alpha(c) (0x0041 <= c && c <= 0x005A)
#define is_ascii_lower_alpha(c) (0x0061 <= c && c <= 0x007A)
#define is_ascii_alpha(c) (is_ascii_upper_alpha(c) || is_ascii_lower_alpha(c))
#define is_ascii_alphanumeric(c) (is_ascii_digit(c) || is_ascii_alpha(c))

bool is_named_character(int c);
bool in_html_namespace_placeholder();
bool adjusted_current_node();

void check_for_duplicate_attributes(lexer * lexer);

#endif
