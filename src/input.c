/*
 * functions to handle reading input from files
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

//TODO: this can probably go in a common_util header somewhere
#define ALLOCATE(ptr, newsize) ptr = realloc(ptr, newsize)

#define STRING_DEFAULT_SIZE 8
#define STRING_GROWTH_RATE 2

#define string_buffer_get(sb, i) sb->data[i]

typedef struct string_buffer {
    size_t length;
    size_t capacity;
    char* data;
} string_buffer;

/*
 * size: the desired initial size (in datatype units)
 * return: a pointer to an initialized string_buffer or NULL
 */
string_buffer* string_buffer_init(size_t size) {
    string_buffer* sb = NULL;
    ALLOCATE(sb, sizeof(string_buffer));
    if (!sb) {
        fprintf(stderr, "string_buffer_init(): \"struct malloc failed\"");
        return NULL;
    }

    ALLOCATE(sb->data, sizeof(char) * size);
    if (!sb->data) {
        fprintf(stderr, "string_buffer_init(): \"data malloc failed\"");
        return NULL;
    }

    sb->length = 0;
    sb->capacity = size;

    return sb;
}

/*
 * frees memory for a string_buffer if it exists
 */
void string_buffer_destroy(string_buffer* sb) {
    if (sb) {
        if (sb->data) {
            free(sb->data);
        }
        free(sb);
    }
}

void string_buffer_grow(string_buffer** sb) {
    if (!sb) {
        fprintf(stderr, "string_buffer_grow(): \"null pointer given as sb\"");
        return;
    }

    (*sb)->capacity *= STRING_GROWTH_RATE;
    ALLOCATE(*sb, (*sb)->capacity);
}

void string_buffer_push_back(string_buffer** sb, char c) {
    if ((*sb)->capacity <= (*sb)->length) {
        string_buffer_grow(sb);
    }

    (*sb)->data[(*sb)->length] = c;
    (*sb)->length += 1;
}

void string_buffer_push_front(string_buffer** sb, char c) {
    if ((*sb)->capacity <= (*sb)->length) {
        string_buffer_grow(sb);
    }

    for (size_t i = (*sb)->length; i > 1; i++) {
        (*sb)->data[i] = (*sb)->data[i-1];
    }

    (*sb)->data[0] = c;
    (*sb)->length += 1;
}

/*
 * append chunk_size bytes from f to sb
 * return: the output from fread() or 0 if the file can't be read from
 */
size_t string_buffer_append_chunk(string_buffer** sb, size_t chunk_size, FILE* f) {
    if (!f) {
        fprintf(stderr, "string_buffer_append_chunk(): \"FILE* f is NULL\"");
        return 0;
    }

    size_t newsize = (*sb)->capacity;
    size_t required_size = (*sb)->length + chunk_size;
    while (newsize <= required_size) {
        newsize *= STRING_GROWTH_RATE;
    }
    if (newsize != (*sb)->capacity) {
        ALLOCATE(((*sb)->data), newsize);
        (*sb)->capacity = newsize;
    }

    size_t res = fread((*sb)->data + (*sb)->length, 1, chunk_size, f);
    (*sb)->length += res; //this only works because size is 1 (chars)
    return res;
}
