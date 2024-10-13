#ifndef INPUT_MODULE
#define INPUT_MODULE
/*
 * functions to handle reading input from files
 */

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>


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
const char* input_system_peekn(size_t num, size_t* out_len);
char input_system_peek();
bool input_system_empty();

string_buffer* string_buffer_init(const size_t size, const char* strdata, const size_t strlen);
void string_buffer_destroy(string_buffer** sb);
void string_buffer_grow(string_buffer** sb);
void string_buffer_grow_to(string_buffer** sb, size_t required_size);
void string_buffer_push_back(string_buffer** sb, const char c);
void string_buffer_push_front(string_buffer** sb, const char c);
void string_buffer_append_raw(string_buffer** sb, const char* strdata, const size_t strlen);
size_t string_buffer_append_chunk(string_buffer** sb, const size_t chunk_size, FILE* f);
char string_buffer_pop_front(string_buffer** sb);

#endif
