#ifndef TEST_MODULE
#define TEST_MODULE

#include <string.h>

#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define COLOR_RESET "\x1b[0m"

extern size_t succeeded;
extern size_t failed;

#define TEST(condition, header) if(condition){fprintf(stdout,GREEN"PASSED: ");succeeded++;}else{fprintf(stdout,RED"FAILED: ");failed++;}fprintf(stdout,"%s\n"COLOR_RESET,header)

#endif
