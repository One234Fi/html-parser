#include "test.h"
#include <stdbool.h>
#include <string.h>
#include "input.h"
#include "common.h"

struct input_system {
    string_buffer* buffer;
    FILE* f;
    bool file_is_open;
};

static struct input_system input = {};
static const size_t CHUNK_SIZE = 1024;

void input_system_read_more();
void normalize_newlines(string_buffer** sb);

void concat_tests();

void run_input_stream_preprocessing_tests() {
    LOG_INFO("run_input_stream_preprocessing_tests()");
    char* test_data = "This is a string!";
    string_buffer* sb = string_buffer_init(18, test_data, 18);
    int res = strncmp(test_data, sb->data, 18);
    TEST(res == 0, "String buffer should equal its input");
    TEST(res != 0, "Test failure should show red");

    char* second_literal = "This is not the same string...";
    res = strncmp(test_data, sb->data, 18);
    TEST(res == 0, "String buffer should equal its input");
    TEST(res != 0, "Test failure should show red");

    size_t t = strlen(second_literal);
    string_buffer_append_raw(&sb, second_literal, t);
    LOG_INFO(sb->data);

    concat_tests();
}

void concat_tests() {
    LOG_INFO("concat_tests()");
    char* str = "";
    size_t strl = strlen(str);
    string_buffer* sb = string_buffer_init(strl, str, strl);

    TEST(sb->length == 0, "Empty string should result in 0 len");
    TEST(sb->capacity == STRING_DEFAULT_SIZE, "Empty string should default cap");

    string_buffer_destroy(&sb);
    TEST(sb == NULL, "Memory should be freed by destroy");


    char* str1 = "Hello, ";
    char* str2 = "World!";
    string_buffer* buf = string_buffer_init(0, str1, strlen(str1));
    TEST(buf->capacity == STRING_DEFAULT_SIZE, "Init size of 0 should result in default capacity");
    TEST(buf->length == strlen(str1), "Init data should be the same length as the input");
    TEST(strncmp(buf->data, str1, strlen(str1)) == 0, "Init data should be the string literal");
    LOG_INFO(str1);
    LOG_INFO(buf->data);

    char* expected_str = "Hello, World!";
    size_t expected_len = strlen(expected_str);

    string_buffer_append_raw(&buf, str2, strlen(str2));
    TEST(strncmp(buf->data, expected_str, expected_len) == 0, "Expected output should match input strings");
    TEST(buf->length == expected_len, "Length should equal the sum of the input lengths");
    LOG_INFO(expected_str);
    LOG_INFO(buf->data);
}


