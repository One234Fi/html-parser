#include "test.h"
#include <stdio.h>

//void run_normalize_newlines_tests();
void color_banner();

int main() {
    color_banner();
//    run_normalize_newlines_tests();
}

void color_banner() {
    fprintf(stdout, 
            "COLOR CHECK:\n"
            RED"RED\n"
            YELLOW"YELLOW\n"
            GREEN"GREEN\n"
            BLUE"BLUE\n"
            COLOR_RESET"COLOR CHECK COMPLETE\n");
}
