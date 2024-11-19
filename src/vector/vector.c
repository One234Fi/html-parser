#include "common.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

#ifndef VEC_DEFAULT_SIZE
#define VEC_DEFAULT_SIZE 8
#endif

#ifndef VEC_GROWTH_FACTOR
#define VEC_GROWTH_FACTOR 2
#endif

struct vector {
    size_t length;
    size_t capacity;
    size_t item_size;
    void * data;
};

vector *
vector_init(size_t item_size) {
    vector * p = MALLOC(sizeof(vector));
    p->data = MALLOC(item_size * VEC_DEFAULT_SIZE);
    p->length = 0;
    p->capacity = VEC_DEFAULT_SIZE;
    p->item_size = item_size;

    return p;
}

void
vector_destroy(vector * v) {
    FREE(v->data);
    FREE(v);
}

void 
vector_push_back(vector * v, void * item) {
    if (v->length >= v->capacity) {
        v->capacity *= VEC_GROWTH_FACTOR;
        v->data = REALLOC(v->data, v->capacity * v->item_size);
    }

    memcpy(
        v->data + v->item_size * v->length,
        item,
        v->item_size
    );
}

const void *
vector_pop_back(vector * v) {
    ASSERT(0 < v->length, "Can't get out of bounds item", NULL);
    return v->data + v->item_size * (v->length - 1);
}

const void *
vector_get(const vector * v, const size_t index) {
    ASSERT(index < v->length, "Can't get out of bounds item", NULL);
    return v->data + v->item_size * index;
}

void grow(void * slice, ptrdiff_t size, ptrdiff_t align, arena * a) {
    struct {
        void * data;
        ptrdiff_t len;
        ptrdiff_t cap;
    } temp;
    memcpy(&temp, slice, sizeof(temp));
    assert(temp.len >= 0);
    assert(temp.cap >= 0);
    assert(temp.len <= temp.cap);

    if (!temp.data) {
        temp.cap = 1;
        temp.data = alloc(a, size * 2, align, temp.cap);
    }
    else if (a->beg == temp.data + size * temp.cap) {
        alloc(a, size, 1, temp.cap); 
    }
    else {
        void * data = alloc(a, size * 2, align, temp.cap);
        memcpy(data, temp.data, size * temp.len);
        temp.data = data;
    }

    temp.cap *= 2;
    memcpy(slice, &temp, sizeof(temp));
}

#undef VEC_DEFAULT_SIZE
#undef VEC_GROWTH_FACTOR
