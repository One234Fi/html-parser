//UNIMPLEMENTED STUFF
#include "tokenizer.h"
#include "error.h"
#include <stdio.h>


void log_error(enum HTML_PARSER_ERROR error) {
    fprintf(stderr, "Unimplemented: log_error\n");
}

void check_for_duplicate_attributes() {
    fprintf(stderr, "Unimplemented: check_for_duplicate_attributes\n");
}

void flush_code_points() {
    fprintf(stderr, "Unimplemented: flush_code_points\n");
}

void create_token(enum TOKEN_TYPE token_type) {
    fprintf(stderr, "Unimplemented: create_token\n");
}

void emit_token(enum TOKEN_TYPE token_type, int c) {
    fprintf(stderr, "Unimplemented: emit_token\n");
}

void emit_current_token() {
    fprintf(stderr, "Unimplemented: emit_current_token\n");
}

void append_to_current_tag_token_name(int c) {
    fprintf(stderr, "Unimplemented: append_to_current_tag_token_name\n");
}

void clear_temporary_buffer() {
    fprintf(stderr, "Unimplemented: clear_temporary_buffer\n");
}

void append_to_temp_buffer(int c) {
    fprintf(stderr, "Unimplemented: append_to_temp_buffer\n");
}

void emit_tokens_in_temp_buffer() {
    fprintf(stderr, "Unimplemented: emit_tokens_in_temp_buffer\n");
}

bool current_token_is_valid() {
    fprintf(stderr, "Unimplemented: current_token_is_valid\n");
    return 0;
}

const char* get_temporary_buffer() {
    fprintf(stderr, "Unimplemented: get_temporary_buffer\n");
    return 0;
}

void start_new_attribute_for_current_tag_token() {
    fprintf(stderr, "Unimplemented: start_new_attribute_for_current_tag_token\n");
}

void append_to_current_tag_token_attribute_name(int c) {
    fprintf(stderr, "Unimplemented: append_to_current_tag_token_attribute_name\n");
}

void append_to_current_tag_token_attribute_value(int c) {
    fprintf(stderr, "Unimplemented: append_to_current_tag_token_attribute_value\n");
}

void set_self_closing_tag_for_current_token(bool b) {
    fprintf(stderr, "Unimplemented: set_self_closing_tag_for_current_token\n");
}

void append_to_current_tag_token_comment_data(int c) {
    fprintf(stderr, "Unimplemented: append_to_current_tag_token_comment_data\n");
}

bool adjusted_current_node() {
    fprintf(stderr, "Unimplemented: adjusted_current_node\n");
    return 0;
}

void set_doctype_token_force_quirks_flag (bool b) {
    fprintf(stderr, "Unimplemented: set_doctype_token_force_quirks_flag\n");
}

void set_current_token_identifier(const char* val, size_t len) {
    fprintf(stderr, "Unimplemented: set_current_token_identifier\n");
}

void append_to_current_tag_token_identifier(int c) {
    fprintf(stderr, "Unimplemented: append_to_current_tag_token_identifier\n");
}

void return_state() {
    fprintf(stderr, "Unimplemented: return_state\n");
}

bool is_named_character(int c) {
    fprintf(stderr, "Unimplemented: is_named_character\n");
    return 0;
}

bool is_part_of_an_attribute() {
    fprintf(stderr, "Unimplemented: is_part_of_an_attribute\n");
    return 0;
}

void interpret_character_reference_name() {
    fprintf(stderr, "Unimplemented: interpret_character_reference_name\n");
}

void set_character_reference_code(int c) {
    fprintf(stderr, "Unimplemented: set_character_reference_code\n");
}

int get_character_reference_code() {
    fprintf(stderr, "Unimplemented: get_character_reference_code\n");
    return 0;
}
