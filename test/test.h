#ifndef TEST_MODULE
#define TEST_MODULE

#include <string.h>
#include "common.h"

extern size_t succeeded;
extern size_t failed;

#define TEST(condition, header) if(condition){fprintf(stdout,GREEN"PASSED: ");succeeded++;}else{fprintf(stdout,RED"FAILED: ");failed++;}fprintf(stdout,"%s\n"COLOR_RESET,header)

#endif
