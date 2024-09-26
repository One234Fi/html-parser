#include "test.h"
#include <stdio.h>

void color_banner();
void run_input_stream_preprocessing_tests();

void (*tests[]) (void) = {
    color_banner,
    run_input_stream_preprocessing_tests
};
static size_t num_tests = sizeof(tests) / sizeof(tests[0]);
size_t succeeded = 0;
size_t failed = 0;

void run_all_tests() {
    for (size_t i = 0; i < num_tests; i++) {
        (*tests[i])();
    }
}

void color_banner() {
    fprintf(stdout, 
            "COLOR CHECK:\n"
            RED"RED\n"
            YELLOW"YELLOW\n"
            GREEN"GREEN\n"
            BLUE"BLUE\n"
            COLOR_RESET"COLOR CHECK COMPLETE\n");
    fprintf(stdout, "=========\n");
    fprintf(stdout, "= TESTS =\n");
    fprintf(stdout, "=========\n");
}

void results() {
    fprintf(stdout, "============\n");
    fprintf(stdout, "Total: %zu\nPassed: %zu\nFailed: %zu\n", num_tests, succeeded, failed);
}

int main() {
    run_all_tests();
    results();
}

