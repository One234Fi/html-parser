#ifndef INPUT_MODULE
#define INPUT_MODULE
/*
 * functions to handle reading input from files
 */

#include "arena/arena.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct string {
    char * data;
    ptrdiff_t len;
    ptrdiff_t cap;
} string;

typedef struct input_system {
    string * buffer;
    char * front;
    ptrdiff_t len; //stack length
} input_system;

input_system input_system_init(const char* filename, arena perm);
int input_system_consume(input_system * s);
void input_system_reconsume(input_system * s);
string input_system_peekn(input_system * s, int n, arena a);
bool input_system_empty(input_system * s);

#endif
