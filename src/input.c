/*
 * functions to handle reading input from files
 */

#include "input.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


struct input_system {
    string_buffer* buffer;
    FILE* f;
    bool file_is_open;
};

static struct input_system input = {};
static const size_t CHUNK_SIZE = 1024;

void input_system_read_more();
void normalize_newlines(string_buffer** sb);


void input_system_init(const char* filename) {
    FILE* f;
    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "INPUT SYSTEM: FAILED TO OPEN FILE\n");
        return;
    }
    input.file_is_open = true;
    input.f = f;

    input.buffer = string_buffer_init(CHUNK_SIZE, NULL, 0);
    string_buffer_append_chunk(&input.buffer, CHUNK_SIZE, input.f);
}

void input_system_read_more() {
    int res = string_buffer_append_chunk(&input.buffer, CHUNK_SIZE, input.f);
    if (res < 0) {
        input.file_is_open = false;
        fclose(input.f);
    }
    normalize_newlines(&input.buffer);
}

void input_system_destroy() {
    if (input.f) {
        fclose(input.f);
    }
    string_buffer_destroy(input.buffer);
}

int input_system_consume() {
    if (input.buffer->length <= 1 && input.file_is_open) {
        input_system_read_more();
    }
    return string_buffer_pop_front(&input.buffer);
}

void input_system_reconsume(const int c) {
    string_buffer_push_front(&input.buffer, c);
}

/*
 * returns a mallocd char buffer. Should be freed manually
 */
const char* input_system_peekn(const size_t num, size_t* out_len) {
    if (input.buffer->length <= num && input.file_is_open) {
        input_system_read_more();
    }

    char * buf = NULL; 
    ALLOCATE(buf, num);
    for (size_t i = 0; i < num; i++) {
        int c = string_buffer_get(input.buffer, i);
        buf[i] = c;
        if (c == EOF) {
            *out_len = i;
            return buf;
        }
    }

    *out_len = num;
    return buf;
}

char input_system_peek() {
    return string_buffer_get(input.buffer, 0);
}

void normalize_newlines(string_buffer** sb) {
    for (size_t i = (*sb)->length; i > 0; i--) {
        if ((*sb)->data[i] == '\r' && (*sb)->data[i+1] == '\n') {
            for (size_t j = i; j < (*sb)->length-1; j++) {
                (*sb)->data[j] = (*sb)->data[j+1];
            }
            (*sb)->length -= 1;
        }
    }

    for (size_t i = 0; i < (*sb)->length; i++) {
        if ((*sb)->data[i] == '\r') {
            (*sb)->data[i] = '\n';
        }
    }
}



/*
 * size: the desired initial size (in datatype units)
 * return: a pointer to an initialized string_buffer or NULL
 */
string_buffer* string_buffer_init(const size_t size, const char* strdata, const size_t strlen) {
    string_buffer* sb = NULL;
    ALLOCATE(sb, sizeof(string_buffer));
    ASSERT(!sb, "struct malloc failed", NULL);

    ALLOCATE(sb->data, sizeof(char) * size);
    if (!sb->data) {
        fprintf(stderr, "string_buffer_init(): \"data malloc failed\"\n");
        return NULL;
    }

    sb->length = 0;
    sb->capacity = size;

    string_buffer_append_raw(&sb, strdata, strlen);

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
    ASSERT(!sb, "NULL pointer passed as sb", );

    (*sb)->capacity *= STRING_GROWTH_RATE;
    ALLOCATE(*sb, (*sb)->capacity);
}

void string_buffer_grow_to(string_buffer** sb, size_t required_size) {
    ASSERT(!sb, "NULL pointer passed as sb", );

    size_t newsize = (*sb)->capacity;
    while (newsize <= required_size) {
        newsize *= STRING_GROWTH_RATE;
    }
    if (newsize != (*sb)->capacity) {
        ALLOCATE(((*sb)->data), newsize);
        (*sb)->capacity = newsize;
    }
}

void string_buffer_append_raw(string_buffer** sb, const char* strdata, const size_t strlen) {
    ASSERT(!sb, "null pointer given as sb", );
    ASSERT(!strdata, "null pointer given as strdata", );

    size_t required_size = (*sb)->length + strlen;
    string_buffer_grow_to(sb, required_size);

    memcpy((*sb)->data + (*sb)->length, strdata, strlen);
    (*sb)->length += strlen;
}

void string_buffer_push_back(string_buffer** sb, const char c) {
    if ((*sb)->capacity <= (*sb)->length) {
        string_buffer_grow(sb);
    }

    (*sb)->data[(*sb)->length] = c;
    (*sb)->length += 1;
}

void string_buffer_push_front(string_buffer** sb, const char c) {
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
size_t string_buffer_append_chunk(string_buffer** sb, const size_t chunk_size, FILE* f) {
    ASSERT(!f, "FILE* f is NULL", 0);

    size_t required_size = (*sb)->length + chunk_size;
    string_buffer_grow_to(sb, required_size);

    size_t res = fread((*sb)->data + (*sb)->length, 1, chunk_size, f);
    (*sb)->length += res; //this only works because size is 1 (chars)
    return res;
}

char string_buffer_pop_front(string_buffer** sb) {
    ASSERT(!sb, "sb is NULL", 0);
    char c = string_buffer_get(*sb, 0);
    for (size_t i = 1; i < (*sb)->length; i++) {
        (*sb)->data[i-1] = (*sb)->data[i];
    }
    (*sb)->length -= 1;

    return c;
}


