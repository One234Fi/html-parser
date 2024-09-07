#ifndef INPUT_MODULE
#define INPUT_MODULE
/*
 * functions to handle reading input from files
 */

#include <stdio.h>
#include <stddef.h>


#define STRING_DEFAULT_SIZE 8
#define STRING_GROWTH_RATE 2

#define string_buffer_get(sb, i) (sb)->data[i]

typedef struct string_buffer {
    size_t length;
    size_t capacity;
    char* data;
} string_buffer;

void input_system_init(const char* filename);
void input_system_destroy();
int input_system_consume();
void input_system_reconsume(int c);
const char* input_system_peekn(int num, size_t* out_len);
char input_system_peek();

string_buffer* string_buffer_init(size_t size);
void string_buffer_destroy(string_buffer* sb);
void string_buffer_grow(string_buffer** sb);
void string_buffer_push_back(string_buffer** sb, char c);
void string_buffer_push_front(string_buffer** sb, char c);
size_t string_buffer_append_chunk(string_buffer** sb, size_t chunk_size, FILE* f);
char string_buffer_pop_front(string_buffer** sb);

#endif
