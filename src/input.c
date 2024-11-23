/*
 * functions to handle reading input from files
 */

#include "arena/arena.h"
#include "common.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "vector/vector.h"
#include "input.h"

void normalize_newlines(string * sb);

bool input_system_empty(input_system * s) {
    return s->len == 0;
}

input_system input_system_init(const char* filename, arena a) {
    input_system s = {};

    FILE* f;
    f = fopen(filename, "rb");
    ASSERT(!f, "FAILED TO OPEN FILE", s);

    //TODO: read entire file for now. Change later if it becomes a problem
    //TODO: this is probably a stupid way to do this
    int c = 0;
    do {
        int c = getc(f);
        *push(s.buffer, &a) = c;
    } while(c != EOF);
    fclose(f);

    s.front = s.buffer->data;
    s.len = s.buffer->len;
    normalize_newlines(s.buffer);
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
    assert(s->front > s->buffer->data);
    s->front--;
    s->len++;
}

string input_system_peekn(input_system * s, int n, arena a) {
    string st = {};
    int l = n < s->len ? n : s->len;
    for (int i = 0; i < l; i++) {
        *push(&st, &a) = s->front[i];
    }
    return st;
}

char input_system_peek(input_system * s) {
    return *s->front;
}

void normalize_newlines(string * sb) {
    for (int i = sb->len; i > 0; i--) {
        if (sb->data[i] == '\r' && sb->data[i+1] == '\n') {
            for (int j = i; j < sb->len-1; j++) {
                sb->data[j] = sb->data[j+1];
            }
            sb->len -= 1;
        }
    }

    //edge case
    if (sb->data[0] == '\r' && sb->data[1] == '\n') {
            for (int j = 0; j < sb->len-1; j++) {
                sb->data[j] = sb->data[j+1];
            }
            sb->len -= 1;
    }

    for (int i = 0; i < sb->len; i++) {
        if (sb->data[i] == '\r') {
            sb->data[i] = '\n';
        }
    }
    sb->data[sb->len] = '\0';
}
