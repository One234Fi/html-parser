/*
 * functions to handle reading input from files
 */

#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


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

    input.buffer = string_buffer_init(CHUNK_SIZE);
    string_buffer_append_chunk(&input.buffer, CHUNK_SIZE, input.f);
}

void input_system_read_more() {
    size_t res = string_buffer_append_chunk(&input.buffer, CHUNK_SIZE, input.f);
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

void input_system_reconsume(int c) {
    string_buffer_push_front(&input.buffer, c);
}

/*
 * returns a mallocd char buffer. Should be freed manually
 */
const char* input_system_peekn(int num, size_t* out_len) {
    if (input.buffer->length <= num && input.file_is_open) {
        input_system_read_more();
    }

    char * buf; 
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
string_buffer* string_buffer_init(size_t size) {
    string_buffer* sb = NULL;
    ALLOCATE(sb, sizeof(string_buffer));
    if (!sb) {
        fprintf(stderr, "string_buffer_init(): \"struct malloc failed\"\n");
        return NULL;
    }

    ALLOCATE(sb->data, sizeof(char) * size);
    if (!sb->data) {
        fprintf(stderr, "string_buffer_init(): \"data malloc failed\"\n");
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
        fprintf(stderr, "string_buffer_grow(): \"null pointer given as sb\"\n");
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
        fprintf(stderr, "string_buffer_append_chunk(): \"FILE* f is NULL\"\n");
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

char string_buffer_pop_front(string_buffer** sb) {
    if (!sb) {
        fprintf(stderr, "string_buffer_pop_front: \"sb is NULL\"\n");
    }
    char c = string_buffer_get(*sb, 0);
    for (size_t i = 1; i < (*sb)->length; i++) {
        (*sb)->data[i-1] = (*sb)->data[i];
    }
    (*sb)->length -= 1;

    return c;
}


