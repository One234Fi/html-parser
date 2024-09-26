#include "test.h"
#include <string.h>
#include "input.h"

void normalize_newlines(string_buffer** sb);


void run_input_stream_preprocessing_tests() {
    char* test_data = "This is a string!";
    string_buffer* sb = string_buffer_init(18);
    for (size_t i = 0; i < 18; i++) {
        string_buffer_push_back(&sb, test_data[i]);
    }
    int res = strncmp(test_data, sb->data, 18);
    TEST(res == 0, "String buffer should equal its input");
    TEST(res != 0, "Test failure should show red");
    fprintf(stdout, "This should be white\n");
}
