#include "test.h"
#include "arena/arena.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "vector/vector.h"

void color_banner();
void run_input_tests();
void print_utf8_file();
void print_ascii_file();


void (*tests[]) (void) = {
    color_banner,
    run_input_tests
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
    fprintf(stdout, "Total: %zu\nPassed: %zu\nFailed: %zu\n", succeeded + failed, succeeded, failed);
}
 

int main() {
    run_all_tests();
    results();

    print_utf8_file();
    print_ascii_file();
}

