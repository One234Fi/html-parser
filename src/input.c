/*
 * functions to handle reading input from files
 */

#include "arena/arena.h"
#include "common.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "input.h"

void normalize_newlines(string * sb);

bool input_system_empty(input_system * s) {
    return s->len == 0;
}

input_system input_system_init(const char* filename, arena * a) {
    input_system s = {0};

    FILE* f;
    f = fopen(filename, "rb");
    assert(f);

    fseek(f, 0L, SEEK_END);
    ptrdiff_t filesize = 0;
    filesize = ftell(f);
    if (filesize < 0) {
        LOG_ERROR("Failed to parse file length");
        exit(EXIT_FAILURE);
    } else if (filesize == 0) {
        LOG_ERROR("File is empty");
        exit(EXIT_SUCCESS);
    }
    fseek(f, 0L, SEEK_SET);

    //TODO: read entire file for now. Change later if it becomes a problem
    string temp = {0};
    temp.data = new(a, char, filesize);
    temp.len = filesize;

    int res = fread(temp.data, 1, filesize, f);
    if (res != filesize) {
        LOG_ERROR("Failed to parse file contents");
        exit(EXIT_FAILURE);
    }
    fclose(f);

    s.buffer = temp;
    s.front = s.buffer.data;
    s.len = s.buffer.len;
    normalize_newlines(&s.buffer);
    return s;
}

int input_system_consume(input_system * s) {
    assert(s->len >= 0);
    char c = *s->front;
    s->front++;
    s->len--;
    return c;
}

void input_system_reconsume(input_system * s) {
    assert(s->front > s->buffer.data);
    s->front--;
    s->len++;
}

string input_system_peekn(input_system * s, int n, arena * a) {
    string st = {0};
    st.data = new(a, char, n);
    st.len = n;

    int l = n < s->len ? n : s->len;
    for (int i = 0; i < l; i++) {
        st.data[i] = s->front[i];
    }
    return st;
}

char input_system_peek(input_system * s) {
    return *s->front;
}

void normalize_newlines(string * sb) {
    string temp = {0};
    temp.data = sb->data;
    temp.len = sb->len;

    for (ptrdiff_t i = temp.len-1; i > 0; i--) {
        if (temp.data[i] == '\r' && temp.data[i+1] == '\n') {
            for (ptrdiff_t j = i; j < temp.len-1; j++) {
                temp.data[j] = temp.data[j+1];
            }
            temp.len -= 1;
        }
    }

    //edge case
    if (temp.data[0] == '\r' && temp.data[1] == '\n') {
            for (ptrdiff_t j = 0; j < temp.len-1; j++) {
                temp.data[j] = temp.data[j+1];
            }
            temp.len -= 1;
    }

    for (ptrdiff_t i = 0; i < temp.len; i++) {
        if (temp.data[i] == '\r') {
            temp.data[i] = '\n';
        }
    }

    *sb = temp;
}
