/*
 * State machine for html tokenization
 */

#include "tokenizer.h"
#include "error.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>


static struct tokenizer tokenizer;

#define UNICODE_REPLACEMENT_CHAR '0xEF' 


//TODO: unimplemented and uncategorized stubs
int consume();
int reconsume(int c);
void set_state(enum TOKENIZER_STATE_TYPE state);
void set_return_state(enum TOKENIZER_STATE_TYPE state);
enum TOKENIZER_STATE_TYPE get_state();
void create_token(enum TOKEN_TYPE token_type);
void emit_token(enum TOKEN_TYPE token_type, int c);
void emit_current_token();
void append_to_current_tag_token_name(int c);
void clear_temporary_buffer();
void append_to_temp_buffer(int c);
void emit_tokens_in_temp_buffer();
bool current_token_is_valid();
const char* get_temporary_buffer();
void start_new_attribute_for_current_tag_token(); 
void append_to_current_tag_token_attribute_name(int c);
void append_to_current_tag_token_attribute_value(int c);
void set_self_closing_tag_for_current_token(bool b);
void append_to_current_tag_token_comment_data(int c);
const char* peek(int num, size_t* out_len);
char peek_one();
bool adjusted_current_node();
bool in_html_namespace();
void set_doctype_token_force_quirks_flag (bool b);
void set_current_token_identifier(const char* val, size_t len);
void append_to_current_tag_token_identifier(int c);

/* 
 * attribute name needs to be compared against already created attribute names, 
 * if there are duplicates, it is a duplicate attribute parse error, and the 
 * new attribute needs to be removed from the token
 */
void check_for_duplicate_attributes();

//state handlers
void data_state();
void rcdata_state();
void rawtext_state();
void script_data_state();
void plaintext_state();
void tag_open_state();
void end_tag_open_state();
void tag_name_state();
void rcdata_less_than_sign_state();
void rcdata_end_tag_open_state();
void rcdata_end_tag_name_state();
void rawtext_less_than_sign_state();
void rawtext_end_tag_open_state();
void rawtext_end_tag_name_state();
void script_data_less_than_sign_state();
void script_data_end_tag_open_state();
void script_data_end_tag_name_state();
void script_data_escape_start_state();
void script_data_escape_start_dash_state();
void script_data_escaped_state();
void script_data_escaped_dash_state();
void script_data_escaped_dash_dash_state();
void script_data_escaped_less_than_sign_state();
void script_data_escaped_end_tag_open_state();
void script_data_escaped_end_tag_name_state();
void script_data_double_escape_start_state();
void script_data_double_escaped_state();
void script_data_double_escaped_dash_state();
void script_data_double_escaped_dash_dash_state();
void script_data_double_escaped_less_than_sign_state();
void script_data_double_escape_end_state();
void before_attribute_name_state();
void attribute_name_state();
void after_attribute_name_state();
void before_attribute_value_state();
void attribute_value_double_quoted_state();
void attribute_value_single_quoted_state();
void attribute_value_unquoted_state();
void after_attribute_value_quoted_state();
void self_closing_start_tag_state();
void bogus_comment_state();
void markup_declaration_open_state();
void comment_start_state();
void comment_start_dash_state();
void comment_state();
void comment_less_than_sign_state();
void comment_less_than_sign_bang_state();
void comment_less_than_sign_bang_dash_state();
void comment_less_than_sign_bang_dash_dash_state();
void comment_end_dash_state();
void comment_end_state();
void comment_end_bang_state();
void doctype_state();
void before_doctype_name_state();
void doctype_name_state();
void after_doctype_name_state();
void after_doctype_public_keyword_state();
void before_doctype_public_identifier_state();
void doctype_public_identifier_double_quoted_state();
void doctype_public_identifier_single_quoted_state();
void after_doctype_public_identifier_state();
void between_doctype_public_and_system_identifiers_state();
void after_doctype_system_keyword_state();
void before_doctype_system_identifier_state();
void doctype_system_identifier_double_quoted_state();
void doctype_system_identifier_single_quoted_state();
void after_doctype_system_identifier_state();
void bogus_doctype_state();
void cdata_section_state();
void cdata_section_bracket_state();
void cdata_section_end_state();
void character_reference_state();
void named_character_reference_state();
void ambiguous_ampersand_state();
void numeric_character_reference_state();
void hexadecimal_character_reference_start_state();
void decimal_character_reference_start_state();
void hexadecimal_character_reference_state();
void decimal_character_reference_state();
void numeric_character_reference_end_state();

void execute() {
    switch (get_state()) {
        case DATA_STATE: data_state(); break;
        case RCDATA_STATE: rcdata_state(); break;
        case RAWTEXT_STATE: rawtext_state(); break;
        case SCRIPT_DATA_STATE: script_data_state(); break;
        case PLAINTEXT_STATE: plaintext_state(); break;
        case TAG_OPEN_STATE: tag_open_state(); break;
        case END_TAG_OPEN_STATE: end_tag_open_state(); break;
        case TAG_NAME_STATE: tag_name_state(); break;
        case RCDATA_LESS_THAN_SIGN_STATE: rcdata_less_than_sign_state(); break;
        case RCDATA_END_TAG_OPEN_STATE: rcdata_end_tag_open_state(); break;
        case RCDATA_END_TAG_NAME_STATE: rcdata_end_tag_name_state(); break;
        case RAWTEXT_LESS_THAN_SIGN_STATE: rawtext_less_than_sign_state(); break;
        case RAWTEXT_END_TAG_OPEN_STATE: rawtext_end_tag_open_state(); break;
        case RAWTEXT_END_TAG_NAME_STATE: rawtext_end_tag_name_state(); break;
        case SCRIPT_DATA_LESS_THAN_SIGN_STATE: script_data_less_than_sign_state(); break;
        case SCRIPT_DATA_END_TAG_OPEN_STATE: script_data_end_tag_open_state(); break;
        case SCRIPT_DATA_END_TAG_NAME_STATE: script_data_end_tag_name_state(); break;
        case SCRIPT_DATA_ESCAPE_START_STATE: script_data_escape_start_state(); break;
        case SCRIPT_DATA_ESCAPE_START_DASH_STATE: script_data_escape_start_dash_state(); break;
        case SCRIPT_DATA_ESCAPED_STATE: script_data_escaped_state(); break;
        case SCRIPT_DATA_ESCAPED_DASH_STATE: script_data_escaped_dash_state(); break;
        case SCRIPT_DATA_ESCAPED_DASH_DASH_STATE: script_data_escaped_dash_dash_state(); break;
        case SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE: script_data_escaped_less_than_sign_state(); break;
        case SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE: script_data_escaped_end_tag_open_state(); break;
        case SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE: script_data_escaped_end_tag_name_state(); break;
        case SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE: script_data_double_escape_start_state(); break;
        case SCRIPT_DATA_DOUBLE_ESCAPED_STATE: script_data_double_escaped_state(); break;
        case SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE: script_data_double_escaped_dash_state(); break;
        case SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE: script_data_double_escaped_dash_dash_state(); break;
        case SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE: script_data_double_escaped_less_than_sign_state(); break;
        case SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE: script_data_double_escape_end_state(); break;
        case BEFORE_ATTRIBUTE_NAME_STATE: before_attribute_name_state(); break;
        case ATTRIBUTE_NAME_STATE: attribute_name_state(); break;
        case AFTER_ATTRIBUTE_NAME_STATE: after_attribute_name_state(); break;
        case BEFORE_ATTRIBUTE_VALUE_STATE: before_attribute_value_state(); break;
        case ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE: attribute_value_double_quoted_state(); break;
        case ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE: attribute_value_single_quoted_state(); break;
        case ATTRIBUTE_VALUE_UNQUOTED_STATE: attribute_value_unquoted_state(); break;
        case AFTER_ATTRIBUTE_VALUE_QUOTED_STATE: after_attribute_value_quoted_state(); break;
        case SELF_CLOSING_START_TAG_STATE: self_closing_start_tag_state(); break;
        case BOGUS_COMMENT_STATE: bogus_comment_state(); break;
        case MARKUP_DECLARATION_OPEN_STATE: markup_declaration_open_state(); break;
        case COMMENT_START_STATE: comment_start_state(); break;
        case COMMENT_START_DASH_STATE: comment_start_dash_state(); break;
        case COMMENT_STATE: comment_state(); break;
        case COMMENT_LESS_THAN_SIGN_STATE: comment_less_than_sign_state(); break;
        case COMMENT_LESS_THAN_SIGN_BANG_STATE: comment_less_than_sign_bang_state(); break;
        case COMMENT_LESS_THAN_SIGN_BANG_DASH_STATE: comment_less_than_sign_bang_dash_state(); break;
        case COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH_STATE: comment_less_than_sign_bang_dash_dash_state(); break;
        case COMMENT_END_DASH_STATE: comment_end_dash_state(); break;
        case COMMENT_END_STATE: comment_end_state(); break;
        case COMMENT_END_BANG_STATE: comment_end_bang_state(); break;
        case DOCTYPE_STATE: doctype_state(); break;
        case BEFORE_DOCTYPE_NAME_STATE: before_doctype_name_state(); break;
        case DOCTYPE_NAME_STATE: doctype_name_state(); break;
        case AFTER_DOCTYPE_NAME_STATE: after_doctype_name_state(); break;
        case AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE: after_doctype_public_keyword_state(); break;
        case BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE: before_doctype_public_identifier_state(); break;
        case DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE: doctype_public_identifier_double_quoted_state(); break;
        case DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE: doctype_public_identifier_single_quoted_state(); break;
        case AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE: after_doctype_public_identifier_state(); break;
        case BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE: between_doctype_public_and_system_identifiers_state(); break;
        case AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE: after_doctype_system_keyword_state(); break;
        case BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE: before_doctype_system_identifier_state(); break;
        case DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE: doctype_system_identifier_double_quoted_state(); break;
        case DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE: doctype_system_identifier_single_quoted_state(); break;
        case AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE: after_doctype_system_identifier_state(); break;
        case BOGUS_DOCTYPE_STATE: bogus_doctype_state(); break;
        case CDATA_SECTION_STATE: cdata_section_state(); break;
        case CDATA_SECTION_BRACKET_STATE: cdata_section_bracket_state(); break;
        case CDATA_SECTION_END_STATE: cdata_section_end_state(); break;
        case CHARACTER_REFERENCE_STATE: character_reference_state(); break;
        case NAMED_CHARACTER_REFERENCE_STATE: named_character_reference_state(); break;
        case AMBIGUOUS_AMPERSAND_STATE: ambiguous_ampersand_state(); break;
        case NUMERIC_CHARACTER_REFERENCE_STATE: numeric_character_reference_state(); break;
        case HEXADECIMAL_CHARACTER_REFERENCE_START_STATE: hexadecimal_character_reference_start_state(); break;
        case DECIMAL_CHARACTER_REFERENCE_START_STATE: decimal_character_reference_start_state(); break;
        case HEXADECIMAL_CHARACTER_REFERENCE_STATE: hexadecimal_character_reference_state(); break;
        case DECIMAL_CHARACTER_REFERENCE_STATE: decimal_character_reference_state(); break;
        case NUMERIC_CHARACTER_REFERENCE_END_STATE: numeric_character_reference_end_state(); break;
        default: log_error(INVALID_TOKENIZER_STATE_ERROR);
    }
}

void data_state() {
    int c = consume();
    switch (c) {
        case '&': 
            set_return_state(DATA_STATE); 
            set_state(CHARACTER_REFERENCE_STATE);
            break;
        case '<': 
            set_state(TAG_OPEN_STATE); 
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            emit_token(CHARACTER, c);
            break;
        case EOF:
            emit_token(END_OF_FILE, c);
            break;
        default:
            emit_token(CHARACTER, c);
    }
}

void rcdata_state() {
    int c = consume();
    switch(c) {
        case '&':
            set_return_state(DATA_STATE); 
            set_state(CHARACTER_REFERENCE_STATE);
            break;
        case '<':
            set_state(RCDATA_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR); 
            break;
        case EOF:
            emit_token(END_OF_FILE, c);
            break;
        default:
            emit_token(CHARACTER, c);
    }
}

void rawtext_state() {
    int c = consume();
    switch (c) {
        case '<':
            set_state(RAWTEXT_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR); 
            break;
        case EOF:
            emit_token(END_OF_FILE, c);
            break;
        default:
            emit_token(CHARACTER, c);
    }
}

void script_data_state() {
    int c = consume();
    switch(c) {
        case '<':
            set_state(SCRIPT_DATA_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR); 
            break;
        case EOF:
            emit_token(END_OF_FILE, c);
            break;
        default:
            emit_token(CHARACTER, c);
    }
}

void plaintext_state() {
    int c = consume();
    switch(c) {
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR); 
            break;
        case EOF:
            emit_token(END_OF_FILE, c);
            break;
        default:
            emit_token(CHARACTER, c);
    }
}

void tag_open_state() {
    int c = consume();
    switch(c) {
        case '!':
            set_state(MARKUP_DECLARATION_OPEN_STATE);
            break;
        case '/':
            set_state(END_TAG_OPEN_STATE);
            break;
        case '?':
            log_error(UNEXPECTED_QUESTION_MARK_INSTEAD_OF_TAG_NAME_PARSE_ERROR);
            create_token(COMMENT);
            reconsume(c);
            set_state(BOGUS_COMMENT_STATE);
            break;
        case EOF:
            log_error(EOF_BEFORE_TAG_NAME_PARSE_ERROR);
            emit_token(CHARACTER, '<');
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            if (isalpha(c)) {
                create_token(START_TAG);
                reconsume(c);
                set_state(TAG_NAME_STATE);
                return;
            } else {
                log_error(INVALID_FIRST_CHARACTER_OF_TAG_NAME_PARSE_ERROR);
                emit_token(CHARACTER, '<');
                reconsume(c);
                set_state(DATA_STATE);
            }
    }
}

void end_tag_open_state() {
    int c = consume();
    switch(c) {
        case '>':
            log_error(MISSING_END_TAG_NAME_PARSE_ERROR);
            set_state(DATA_STATE);
            break;
        case EOF:
            log_error(EOF_BEFORE_TAG_NAME_PARSE_ERROR);
            emit_token(CHARACTER, '/');
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            if (isalpha(c)) {
                create_token(END_TAG);
                reconsume(c);
                set_state(TAG_NAME_STATE);
            } else {
                log_error(INVALID_FIRST_CHARACTER_OF_TAG_NAME_PARSE_ERROR);
                create_token(COMMENT);
                reconsume(c);
                set_state(BOGUS_COMMENT_STATE);
            }
    }
}

void tag_name_state() {
    int c = consume();
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(BEFORE_ATTRIBUTE_NAME_STATE);
            break;
        case '/':
            set_state(SELF_CLOSING_START_TAG_STATE);
            break;
        case '>':
            set_state(DATA_STATE);
            emit_current_token(); //emit current tag token
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            //append UNICODE_REPLACEMENT_CHAR to current tag token's tag name
            break;
        case EOF:
            log_error(EOF_IN_TAG_PARSE_ERROR);
            emit_token(END_OF_FILE, c);
            break;
        default:
            if (isalpha(c)) {
                append_to_current_tag_token_name(tolower(c));
            } else {
                append_to_current_tag_token_name(c);
            }
    }
}

void rcdata_less_than_sign_state() {
    int c = consume();
    if (c == '/') {
        clear_temporary_buffer(); //set temp buffer to ""
        set_state(RCDATA_END_TAG_OPEN_STATE);
    } else {
        emit_token(CHARACTER, '<');
        reconsume(c);
        set_state(RCDATA_STATE);
    }
}

void rcdata_end_tag_open_state() {
    int c = consume();
    if (isalpha(c)) {
        create_token(END_TAG);
        reconsume(c);
        set_state(RCDATA_END_TAG_NAME_STATE);
    } else {
        emit_token(CHARACTER, '<');
        emit_token(CHARACTER, '/');
        reconsume(c);
        rcdata_state();
    }
}

void rcdata_end_tag_name_state() {
    int c = consume();
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(RCDATA_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(SELF_CLOSING_START_TAG_STATE);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(RCDATA_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(DATA_STATE);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(RCDATA_STATE);
            }
            break;
        default:
            if (isalpha(c) && isupper(c)) {
                append_to_current_tag_token_name(tolower(c));
                append_to_temp_buffer(c);
            } else if (isalpha(c) && islower(c)) {
                append_to_current_tag_token_name(c);
                append_to_temp_buffer(c);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(RCDATA_STATE);
            }
    }
}

void rawtext_less_than_sign_state() {
    int c = consume();
    if (c == '/') {
        clear_temporary_buffer();
        set_state(RAWTEXT_END_TAG_OPEN_STATE);
    } else {
        emit_token(CHARACTER, '<');
        reconsume(c);
        set_state(RAWTEXT_STATE);
    }
}

void rawtext_end_tag_open_state() {
    int c = consume();
    if (isalpha(c)) {
        create_token(END_TAG);
        reconsume(c);
        set_state(SCRIPT_DATA_END_TAG_NAME_STATE);
    } else {
        emit_token(CHARACTER, '/');
        reconsume(c);
        set_state(SCRIPT_DATA_STATE);
    }
}

void script_data_end_tag_name_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(SCRIPT_DATA_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(SELF_CLOSING_START_TAG_STATE);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(SCRIPT_DATA_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(DATA_STATE);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(SCRIPT_DATA_STATE);
            }
            break;
        default:
            if (isalpha(c) && isupper(c)) {
                append_to_current_tag_token_name(tolower(c));
                append_to_temp_buffer(c);
            } else if (isalpha(c) && islower(c)) {
                append_to_current_tag_token_name(c);
                append_to_temp_buffer(c);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(SCRIPT_DATA_STATE);
            }
    }
}

void script_data_escape_start_state() {
    int c = consume();
    if (c == '-') {
        set_state(SCRIPT_DATA_ESCAPE_START_DASH_STATE);
        emit_token(CHARACTER, '-');
    } else {
        reconsume(c);
        set_state(SCRIPT_DATA_STATE);
    }
}

void script_data_escape_start_dash_state() {
    int c = consume();
    if (c == '-') {
        set_state(SCRIPT_DATA_ESCAPED_DASH_DASH_STATE);
        emit_token(CHARACTER, '-');
    } else {
        reconsume(c);
        set_state(SCRIPT_DATA_STATE);
    }
}

void script_data_escaped_state() {
    int c = consume();
    switch (c) {
        case '-':
            set_state(SCRIPT_DATA_ESCAPED_DASH_STATE);
            emit_token(CHARACTER, '-');
            break;
        case '<':
            set_state(SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF: 
            log_error(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            emit_token(CHARACTER, c);
    }
}

void script_data_escaped_dash_state() {
    int c = consume();
    switch (c) {
        case '-': 
            set_state(SCRIPT_DATA_ESCAPED_DASH_DASH_STATE);
            break;
        case '<': 
            set_state(SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            set_state(SCRIPT_DATA_ESCAPED_STATE);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF: 
            log_error(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            set_state(SCRIPT_DATA_ESCAPED_STATE);
            emit_token(CHARACTER, c);
    }
}

void script_data_escaped_dash_dash_state() {
    int c = consume();
    switch (c) {
        case '-': 
            emit_token(CHARACTER, '-');
            break;
        case '<': 
            set_state(SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN_STATE);
            break;
        case '>': 
            set_state(SCRIPT_DATA_STATE);
            emit_token(CHARACTER, '>');
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            set_state(SCRIPT_DATA_ESCAPED_STATE);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF: 
            log_error(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            set_state(SCRIPT_DATA_ESCAPED_STATE);
            emit_token(CHARACTER, c);
    }
}

void script_data_escaped_less_than_sign_state() {
    int c = consume();
    switch (c) {
        case '/':
            clear_temporary_buffer();
            set_state(SCRIPT_DATA_ESCAPED_END_TAG_OPEN_STATE);
            break;
        default:
            if (isalpha(c)) {
                clear_temporary_buffer();
                emit_token(CHARACTER, '<');
                reconsume(c);
                set_state(SCRIPT_DATA_DOUBLE_ESCAPE_START_STATE);
            } else {
                emit_token(CHARACTER, '<');
                reconsume(c);
                set_state(SCRIPT_DATA_ESCAPED_STATE);
            }
    }
}

void script_data_escaped_end_tag_open_state() {
    int c = consume();
    if (isalpha(c)) {
        create_token(END_TAG);
        reconsume(c);
        set_state(SCRIPT_DATA_ESCAPED_END_TAG_NAME_STATE);
    } else {
        emit_token(CHARACTER, '<');
        emit_token(CHARACTER, '/');
        reconsume(c);
        set_state(SCRIPT_DATA_ESCAPED_STATE);
    }
}

void script_data_escaped_end_tag_name_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(BEFORE_ATTRIBUTE_NAME_STATE);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(SCRIPT_DATA_ESCAPED_STATE);
            }
            break;
        case '/':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(SELF_CLOSING_START_TAG_STATE);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(SCRIPT_DATA_ESCAPED_STATE);
            }
            break;
        case '>':
            if (current_token_is_valid()) { //current token should be an end tag
                set_state(DATA_STATE);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(SCRIPT_DATA_ESCAPED_STATE);
            }
            break;
        default:
            if (isalpha(c) && isupper(c)) {
                append_to_current_tag_token_name(tolower(c));
                append_to_temp_buffer(c);
            } else if (isalpha(c) && islower(c)) {
                append_to_current_tag_token_name(c);
                append_to_temp_buffer(c);
            } else {
                emit_token(CHARACTER, '<');
                emit_token(CHARACTER, '/');
                emit_tokens_in_temp_buffer(); 
                reconsume(c);
                set_state(SCRIPT_DATA_ESCAPED_STATE);
            }
    }
}

void script_data_double_escape_start_state() {
    int c = consume();
    switch(c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
            if (strncmp(get_temporary_buffer(), "script", 6) == 0) {
                set_state(SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            } else {
                set_state(SCRIPT_DATA_ESCAPED_STATE);
            }
            emit_token(CHARACTER, c);
            break;
        default:
            if (isalpha(c) && isupper(c)) {
                append_to_temp_buffer(tolower(c));
                emit_token(CHARACTER, c);
            } else if (isalpha(c) && islower(c)) {
                append_to_temp_buffer(c);
                emit_token(CHARACTER, c);
            } else {
                reconsume(c);
                set_state(SCRIPT_DATA_ESCAPED_STATE);
            }
    }
}

void script_data_double_escaped_state() {
    int c = consume();
    switch(c) {
        case '-':
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_DASH_STATE);
            emit_token(CHARACTER, '-');
            break;
        case '<':
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE);
            emit_token(CHARACTER, '<');
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            log_error(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            emit_token(CHARACTER, c);
    }
}

void script_data_double_escaped_dash_state() {
    int c = consume();
    switch(c) {
        case '-':
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH_STATE);
            emit_token(CHARACTER, '-');
            break;
        case '<':
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE);
            emit_token(CHARACTER, '<');
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            log_error(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            emit_token(CHARACTER, c);
    }
}

void script_data_double_escaped_dash_dash_state() {
    int c = consume();
    switch (c) {
        case '-':
            emit_token(CHARACTER, '-');
            break;
        case '<':
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN_STATE);
            emit_token(CHARACTER, '<');
            break;
        case '>':
            set_state(SCRIPT_DATA_STATE);
            emit_token(CHARACTER, '>');
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            emit_token(CHARACTER, UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            log_error(EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            emit_token(CHARACTER, c);
    }
}

void script_data_double_escaped_less_than_sign_state() {
    int c = consume();
    switch (c) {
        case '/':
            clear_temporary_buffer();
            set_state(SCRIPT_DATA_DOUBLE_ESCAPE_END_STATE);
            emit_token(CHARACTER, '/');
            break;
        default:
            reconsume(c);
            set_state(SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
    }
}

void script_data_double_escape_end_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
            if (strncmp(get_temporary_buffer(), "script", 6) == 0) {
                set_state(SCRIPT_DATA_ESCAPED_STATE);
            } else {
                set_state(SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            }
            emit_token(CHARACTER, c);
            break;
        default:
            if (isalpha(c) && isupper(c)) {
                append_to_temp_buffer(tolower(c));
                emit_token(CHARACTER, c);
            } else if (isalpha(c) && islower(c)) {
                append_to_temp_buffer(c);
                emit_token(CHARACTER, c);
            } else {
                reconsume(c);
                set_state(SCRIPT_DATA_DOUBLE_ESCAPED_STATE);
            }
    }
}

void before_attribute_name_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
        case EOF:
            reconsume(c);
            set_state(AFTER_ATTRIBUTE_NAME_STATE);
            break;
        case '=':
            log_error(UNEXPECTED_EQUALS_SIGN_BEFORE_ATTRIBUTE_NAME_PARSE_ERROR);
            start_new_attribute_for_current_tag_token();
            append_to_current_tag_token_attribute_name(c);
            set_state(ATTRIBUTE_NAME_STATE);
            break;
        default:
            start_new_attribute_for_current_tag_token();
            reconsume(c);
            set_state(ATTRIBUTE_NAME_STATE);
    }
}

void attribute_name_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
        case '/':
        case '>':
        case EOF:
            reconsume(c);
            set_state(AFTER_ATTRIBUTE_NAME_STATE);
            check_for_duplicate_attributes();
            break;
        case '=':
            set_state(BEFORE_ATTRIBUTE_VALUE_STATE);
            check_for_duplicate_attributes();
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            append_to_current_tag_token_attribute_name(UNICODE_REPLACEMENT_CHAR);
            break;
        case '"':
        case '\'':
        case '<':
            log_error(UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR);
            append_to_current_tag_token_attribute_name(c);
            break;
        default:
            if (isalpha(c) && isupper(c)) {
                append_to_current_tag_token_attribute_name(tolower(c));
            } else {
                append_to_current_tag_token_attribute_name(c);
            }
    }
}

void after_attribute_name_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore these characters
            break;
        case '/':
            set_state(SELF_CLOSING_START_TAG_STATE);
            break;
        case '=':
            set_state(BEFORE_ATTRIBUTE_VALUE_STATE);
            break;
        case '>':
            set_state(DATA_STATE);
            break;
        case EOF:
            log_error(EOF_IN_TAG_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            start_new_attribute_for_current_tag_token();
            reconsume(c);
            set_state(ATTRIBUTE_NAME_STATE);
    }
}

void before_attribute_value_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore these characters
            break;
        case '"':
            set_state(ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_state(ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE);
            break;
        case '>':
            log_error(MISSING_ATTRIBUTE_VALUE_PARSE_ERROR);
            set_state(DATA_STATE);
            emit_current_token();
            break;
        default:
            reconsume(c);
            set_state(ATTRIBUTE_VALUE_UNQUOTED_STATE);
    }
}

void attribute_value_double_quoted_state() {
    int c = consume();
    switch (c) {
        case '"':
            set_state(AFTER_ATTRIBUTE_VALUE_QUOTED_STATE);
            break;
        case '&':
            set_return_state(ATTRIBUTE_VALUE_DOUBLE_QUOTED_STATE);
            set_state(CHARACTER_REFERENCE_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR);
            append_to_current_tag_token_attribute_value(UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            log_error(EOF_IN_TAG_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_attribute_value(c);
    }
}

void attribute_value_single_quoted_state() {
    int c = consume();
    switch (c) {
        case '\'':
            set_state(AFTER_ATTRIBUTE_VALUE_QUOTED_STATE);
            break;
        case '&':
            set_return_state(ATTRIBUTE_VALUE_SINGLE_QUOTED_STATE);
            set_state(CHARACTER_REFERENCE_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            append_to_current_tag_token_attribute_value(UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            log_error(EOF_IN_TAG_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_attribute_value(c);
    }
}

void attribute_value_unquoted_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(BEFORE_ATTRIBUTE_NAME_STATE);
            break;
        case '&':
            set_return_state(ATTRIBUTE_VALUE_UNQUOTED_STATE);
            set_state(CHARACTER_REFERENCE_STATE);
            break;
        case '>':
            set_state(DATA_STATE);
            emit_current_token();
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            append_to_current_tag_token_attribute_value(UNICODE_REPLACEMENT_CHAR);
            break;
        case '"':
        case '\'':
        case '<':
        case '=':
        case '`':
            log_error(UNEXPECTED_CHARACTER_IN_UNQUOTED_ATTRIBUTE_VALUE_PARSE_ERROR);
            append_to_current_tag_token_attribute_value(c);
            break;
        case EOF:
            log_error(EOF_IN_TAG_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_attribute_value(c);
    }
}

void after_attribute_value_quoted_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(BEFORE_ATTRIBUTE_NAME_STATE);
            break;
        case '/':
            set_state(SELF_CLOSING_START_TAG_STATE);
            break;
        case EOF:
            log_error(EOF_IN_TAG_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            log_error(MISSING_WHITESPACE_BETWEEN_ATTRIBUTES_PARSE_ERROR);
            reconsume(c);
            set_state(BEFORE_ATTRIBUTE_NAME_STATE);
    }
}

void self_closing_start_tag_state() {
    int c = consume();
    switch (c) {
        case '>':
            set_self_closing_tag_for_current_token(true);
            set_state(DATA_STATE);
            emit_current_token();
            break;
        case EOF:
            log_error(EOF_IN_TAG_PARSE_ERROR);
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            log_error(UNEXPECTED_SOLIDUS_IN_TAG_PARSE_ERROR);
            reconsume(c);
            set_state(BEFORE_ATTRIBUTE_NAME_STATE);
    }
}

void bogus_comment_state() {
    int c = consume();
    switch (c) {
        case '>':
            set_state(DATA_STATE);
            emit_current_token(); //should be a comment token
            break;
        case EOF:
            emit_current_token();
            emit_token(END_OF_FILE, EOF);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            append_to_current_tag_token_comment_data(UNICODE_REPLACEMENT_CHAR);
            break;
        default:
            append_to_current_tag_token_comment_data(c);
    }
}

void markup_declaration_open_state() {
    size_t buf_length = 0;
    const char * buf = peek(7, &buf_length);

    if (buf[0] == '-' && buf[1] == '-') {
        consume();
        consume();
        create_token(COMMENT);
        set_state(COMMENT_START_STATE);
        return;
    } 

    if (buf_length == 7) {
        if (strncmp(buf, "[CDATA[", buf_length) == 0) {
            for (size_t i = 0; i < buf_length; i++) {
                consume();
            }

            if (adjusted_current_node() && !in_html_namespace()) {
                set_state(CDATA_SECTION_STATE);
            } else {
                log_error(CDATA_IN_HTML_CONTENT_PARSE_ERROR);
                create_token(COMMENT);
                append_to_current_tag_token_comment_data('[');
                append_to_current_tag_token_comment_data('C');
                append_to_current_tag_token_comment_data('D');
                append_to_current_tag_token_comment_data('A');
                append_to_current_tag_token_comment_data('T');
                append_to_current_tag_token_comment_data('A');
                append_to_current_tag_token_comment_data('[');
                set_state(BOGUS_COMMENT_STATE);
            }
            return;
        }

        char lowercase_buf[buf_length] = {};
        for (size_t i = 0; i < buf_length; i++) {
            lowercase_buf[i] = tolower(buf[i]);
        }
        if (strncmp(lowercase_buf, "doctype", buf_length) == 0) {
            for (size_t i = 0; i < buf_length; i++) {
                consume();
            }
            set_state(DOCTYPE_STATE);
            return;
        }
    }

    log_error(INCORRECTLY_OPENED_COMMENT_PARSE_ERROR);
    create_token(COMMENT);
    set_state(BOGUS_COMMENT_STATE);
}

void comment_start_state() {
    int c = consume();
    switch (c) {
        case '-':
            set_state(COMMENT_START_DASH_STATE);
            break;
        case '>':
            log_error(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR);
            set_state(DATA_STATE);
            emit_current_token();
            break;
        default:
            reconsume(c);
            set_state(COMMENT_STATE);
    }
}

void comment_start_dash_state() {
    int c = consume();
    switch (c) {
        case '-':
            set_state(COMMENT_END_STATE);
            break;
        case '>':
            log_error(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR);
            set_state(DATA_STATE);
            emit_current_token(); //TODO: should be a comment token
            break;
        case EOF:
            log_error(EOF_IN_COMMENT_PARSE_ERROR);
            emit_current_token(); //TODO: should be a comment token
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_comment_data('-');
            reconsume(c);
            set_state(COMMENT_STATE);
    }
}

void comment_state() {
    int c = consume();
    switch (c) {
        case '<':
            append_to_current_tag_token_comment_data(c);
            set_state(COMMENT_LESS_THAN_SIGN_STATE);
            break;
        case '-':
            set_state(COMMENT_END_DASH_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            append_to_current_tag_token_comment_data(UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            log_error(EOF_IN_COMMENT_PARSE_ERROR);
            emit_current_token(); //TODO: should be a comment token
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_comment_data(c);
    }
}

void comment_less_than_sign_state() {
    int c = consume();
    switch (c) {
        case '!':
            append_to_current_tag_token_comment_data(c);
            set_state(COMMENT_LESS_THAN_SIGN_BANG_STATE);
            break;
        case '<':
            append_to_current_tag_token_comment_data(c);
            break;
        default:
            reconsume(c);
            set_state(COMMENT_STATE);
    }
}

void comment_less_than_sign_bang_state() {
    int c = consume();
    switch (c) {
        case '-':
            set_state(COMMENT_LESS_THAN_SIGN_BANG_DASH_STATE);
            break;
        default:
            reconsume(c);
            set_state(COMMENT_STATE);
    }
}

void comment_less_than_sign_bang_dash_state() {
    int c = consume();
    switch (c) {
        case '-':
            set_state(COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH_STATE);
            break;
        default:
            reconsume(c);
            set_state(COMMENT_END_DASH_STATE);
    }
}

void comment_less_than_sign_bang_dash_dash_state() {
    int c = consume();
    switch (c) {
        case '>':
        case EOF:
            reconsume(c);
            set_state(COMMENT_END_STATE);
            break;
        default:
            log_error(NESTED_COMMENT_PARSE_ERROR);
            reconsume(c);
            set_state(COMMENT_END_STATE);
    }
}

void comment_end_dash_state() {
    int c = consume();
    switch (c) {
        case '-':
            set_state(COMMENT_END_STATE);
            break;
        case EOF:
            log_error(EOF_IN_COMMENT_PARSE_ERROR);
            emit_current_token(); //TODO: should be comment token
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_comment_data('-');
            reconsume(c);
            set_state(COMMENT_STATE);
    }
}

void comment_end_state() {
    int c = consume();
    switch (c) {
        case '>':
            set_state(DATA_STATE);
            emit_current_token(); //TODO: comment token
            break;
        case '!':
            set_state(COMMENT_END_BANG_STATE);
            break;
        case '-':
            append_to_current_tag_token_comment_data('-');
            break;
        case EOF:
            log_error(EOF_IN_COMMENT_PARSE_ERROR);
            emit_current_token(); //TODO: comment token
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('-');
            reconsume(c);
            set_state(COMMENT_STATE);
    }
}

void comment_end_bang_state() {
    int c = consume();
    switch (c) {
        case '-':
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('!');
            set_state(COMMENT_END_DASH_STATE);
            break;
        case '>':
            log_error(INCORRECTLY_CLOSED_COMMENT_PARSE_ERROR);
            set_state(DATA_STATE);
            emit_current_token(); //TODO: comment
            break;
        case EOF:
            log_error(EOF_IN_COMMENT_PARSE_ERROR);
            emit_current_token(); //TODO: comment
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('-');
            append_to_current_tag_token_comment_data('!');
            reconsume(c);
            set_state(COMMENT_STATE);
    }
}

void doctype_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(BEFORE_DOCTYPE_NAME_STATE);
            break;
        case '>':
            reconsume(c);
            set_state(BEFORE_DOCTYPE_NAME_STATE);
            break;
        case EOF:
            log_error(EOF_IN_COMMENT_PARSE_ERROR);
            create_token(DOCTYPE);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); 
            emit_token(END_OF_FILE, EOF);
        default:
            log_error(MISSING_WHITESPACE_BEFORE_DOCTYPE_NAME_PARSE_ERROR);
            reconsume(c);
            set_state(BEFORE_DOCTYPE_NAME_STATE);
    }
}

void before_doctype_name_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            // intentionally ignore character
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            create_token(DOCTYPE);
            append_to_current_tag_token_name(UNICODE_REPLACEMENT_CHAR);
            set_state(DOCTYPE_NAME_STATE);
            break;
        case '>':
            log_error(MISSING_DOCTYPE_NAME_PARSE_ERROR);
            create_token(DOCTYPE);
            set_doctype_token_force_quirks_flag(true);
            set_state(DATA_STATE);
            emit_current_token();
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            create_token(DOCTYPE);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token();
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            if (isalpha(c) && isupper(c)) {
                create_token(DOCTYPE);
                append_to_current_tag_token_name(tolower(c));
                set_state(DOCTYPE_NAME_STATE);
            } else {
                create_token(DOCTYPE);
                append_to_current_tag_token_name(c);
                set_state(DOCTYPE_NAME_STATE);
            }
    }
}

void doctype_name_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(AFTER_DOCTYPE_NAME_STATE);
            break;
        case '>':
            set_state(DATA_STATE);
            emit_current_token(); // TODO: should be doctype token
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            append_to_current_tag_token_name(UNICODE_REPLACEMENT_CHAR);
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); // TODO: doctype token
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            if (isalpha(c) && isupper(c)) {
                append_to_current_tag_token_name(tolower(c));
            } else {
                append_to_current_tag_token_name(c);
            }
    }
}

void after_doctype_name_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            // intentionally ignore character
            return;
        case '>':
            set_state(DATA_STATE);
            emit_current_token(); // TODO: should be doctype token
            return;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); // TODO: doctype token
            emit_token(END_OF_FILE, EOF);
            return;
    }

    char buf[6] = {};
    buf[0] = tolower(c);
    buf[1] = tolower(peek_one());
    buf[2] = tolower(peek_one());
    buf[3] = tolower(peek_one());
    buf[4] = tolower(peek_one());
    buf[5] = tolower(peek_one());

    if (strncmp(buf, "public", 6) == 0) {
        for (size_t i = 0; i < 5; i++) {
            consume();
        }
        set_state(AFTER_DOCTYPE_PUBLIC_KEYWORD_STATE);
        return;
    }  

    if (strncmp(buf, "system", 6) == 0) {
        for (size_t i = 0; i < 5; i++) {
            consume();
        }
        set_state(AFTER_DOCTYPE_SYSTEM_KEYWORD_STATE);
        return;
    }

    log_error(INVALID_CHARACTER_SEQUENCE_AFTER_DOCTYPE_NAME_PARSE_ERROR);
    set_doctype_token_force_quirks_flag(true);
    reconsume(c);
    set_state(BOGUS_DOCTYPE_STATE);
}

void after_doctype_public_keyword_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_STATE);
            break;
        case '"':
            log_error(MISSING_WHITESPACE_AFTER_DOCTYPE_PUBLIC_KEYWORD_PARSE_ERROR);
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            log_error(MISSING_WHITESPACE_AFTER_DOCTYPE_PUBLIC_KEYWORD_PARSE_ERROR);
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            log_error(MISSING_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            set_state(DATA_STATE);
            emit_current_token(); //TODO: Should be doctype
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); // TODO: doctype token
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            log_error(MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            reconsume(c);
            set_state(BOGUS_DOCTYPE_STATE);
    }
}

void before_doctype_public_identifier_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore character
            break;
        case '"':
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            log_error(MISSING_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            set_state(DATA_STATE);
            emit_current_token(); //TODO: Should be doctype
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); // TODO: doctype token
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            log_error(MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            reconsume(c);
            set_state(BOGUS_DOCTYPE_STATE);
    }
}

void doctype_public_identifer_double_quoted_state() {
    int c = consume();
    switch (c) {
        case '"':
            set_state(AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE);
            break;
        case '>':
            log_error(ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            set_state(DATA_STATE);
            emit_current_token(); //TODO: should be doctype
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); //TODO: should be doctype
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_identifier(c);
    }
}

void doctype_public_identifier_single_quoted_state() {
    int c = consume();
    switch (c) {
        case '"':
            set_state(AFTER_DOCTYPE_PUBLIC_IDENTIFIER_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            append_to_current_tag_token_identifier(UNICODE_REPLACEMENT_CHAR);
            break;
        case '>':
            log_error(ABRUPT_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            set_state(DATA_STATE);
            emit_current_token(); //TODO: doctype
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); //TODO: should be doctype
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_identifier(c);
    }
}

void after_doctype_public_identifier_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_STATE);
            break;
        case '>':
            set_state(DATA_STATE);
            emit_current_token(); //TODO: doctype
            break;
        case '"':
            log_error(MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_PARSE_ERROR);
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            log_error(MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_PARSE_ERROR);
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); //TODO: should be doctype
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            log_error(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            reconsume(c);
            set_state(BOGUS_DOCTYPE_STATE);
    }
}

void between_doctype_public_and_system_identifiers_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            // intentionally ignore character
            break;
        case '>':
            set_state(DATA_STATE);
            emit_current_token(); //TODO: doctype
            break;
        case '"':
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); //TODO: should be doctype
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            log_error(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            reconsume(c);
            set_state(BOGUS_DOCTYPE_STATE);
    }
}

void after_doctype_system_keyword_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            set_state(BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_STATE);
            break;
        case '"':
            log_error(MISSING_WHITESPACE_AFTER_DOCTYPE_SYSTEM_KEYWORD_PARSE_ERROR);
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            log_error(MISSING_WHITESPACE_AFTER_DOCTYPE_SYSTEM_KEYWORD_PARSE_ERROR);
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            log_error(MISSING_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); //TODO: doctype
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); //TODO: should be doctype
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            log_error(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            reconsume(c);
            set_state(BOGUS_DOCTYPE_STATE);
    }
}

void before_doctype_system_identifier_state() {
    int c = consume();
    switch (c) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            //intentionally ignore character
            break;
        case '"':
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED_STATE);
            break;
        case '\'':
            set_current_token_identifier("", 0);
            set_state(DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED_STATE);
            break;
        case '>':
            log_error(MISSING_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            set_state(DATA_STATE);
            emit_current_token(); //TODO: doctype
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); //TODO: doctype
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            log_error(MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            reconsume(c);
            set_state(BOGUS_DOCTYPE_STATE);
            break;
    }
}

void doctype_system_identifier_double_quoted_state() {
    int c = consume();
    switch (c) {
        case '"':
            set_state(AFTER_DOCTYPE_SYSTEM_IDENTIFIER_STATE);
            break;
        case '\0':
            log_error(UNEXPECTED_NULL_CHARACTER_PARSE_ERROR);
            append_to_current_tag_token_identifier(UNICODE_REPLACEMENT_CHAR);
            break;
        case '>':
            log_error(ABRUPT_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            set_state(DATA_STATE);
            emit_current_token(); //TODO: doctype
            break;
        case EOF:
            log_error(EOF_IN_DOCTYPE_PARSE_ERROR);
            set_doctype_token_force_quirks_flag(true);
            emit_current_token(); //TODO: doctype
            emit_token(END_OF_FILE, EOF);
            break;
        default:
            append_to_current_tag_token_identifier(c);
    }
}
