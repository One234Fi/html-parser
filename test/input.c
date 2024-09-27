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

void check_logging() {
    LOG_INFO("This is a message");
    LOG_INFO("This is another message");
    LOG_WARN("Uh-oh");
    LOG_INFO("This is another message");
    LOG_ERROR("This is another message");
}

void run_input_stream_preprocessing_tests() {
    check_logging();
    char* test_data = "This is a string!";
    string_buffer* sb = string_buffer_init(18, test_data, 18);
    int res = strncmp(test_data, sb->data, 18);
    TEST(res == 0, "String buffer should equal its input");
    TEST(res != 0, "Test failure should show red");

    char* second_literal = "This is not the same string...";
    res = strncmp(test_data, sb->data, 18);
    TEST(res == 0, "String buffer should equal its input");
    TEST(res != 0, "Test failure should show red");

    string_buffer_append_raw(&sb, second_literal, 31);
    LOG_INFO(sb->data);
    sb->data[17] = '1';
    sb->data[18] = '1';
    sb->data[19] = '1';
    LOG_INFO(sb->data);
}

