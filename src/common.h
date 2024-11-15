#ifndef COMMON_MODULE
#define COMMON_MODULE

#include <stdlib.h>

#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

#define LOG_INFO(message) fprintf(stderr, BLUE"%s, %s, %d:"COLOR_RESET" \"%s\"\n", __FILE__, __func__, __LINE__, message)
#define LOG_ERROR(message) fprintf(stderr, BLUE"%s, %s, %d:"RED" \"%s\"\n"COLOR_RESET, __FILE__, __func__, __LINE__, message)
#define LOG_WARN(message) fprintf(stderr, BLUE"%s, %s, %d:"YELLOW" \"%s\"\n"COLOR_RESET, __FILE__, __func__, __LINE__, message)
#define ASSERT(expr, message, return_val) if (!(expr)) { LOG_ERROR(message); return return_val; } 

#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

#endif
