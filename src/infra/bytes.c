#include "common.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INFRA_BYTE_SEQUENCE_DEFAULT_SIZE 8
#define INFRA_BYTE_SEQUENCE_GROWTH_RATE 2

infra_byte infra_byte_to_lowercase(infra_byte byte) {
    if (0x41 <= byte && byte <= 0x5A) {
        return byte + 0x20;
    }
    return byte;
}

infra_byte infra_byte_to_uppercase(infra_byte byte) {
    if (0x61 <= byte && byte <= 0x7A) {
        return byte - 0x20;
    }
    return byte;
}

infra_byte_sequence infra_byte_sequence_to_lowercase(infra_byte_sequence bytes) {
    ASSERT(!bytes, "Bytes sequence is NULL", NULL);
    ASSERT(!bytes->data, "Bytes sequence data is NULL", NULL);
    infra_byte_sequence seq = infra_byte_sequence_init(bytes->capacity, NULL, 0);
    for (size_t i = 0; i < bytes->length; i++) {
        seq->data[i] = infra_byte_to_lowercase(bytes->data[i]);
    }
    return seq;
}

infra_byte_sequence infra_byte_sequence_to_uppercase(infra_byte_sequence bytes) {
    ASSERT(!bytes, "Bytes sequence is NULL", NULL);
    ASSERT(!bytes->data, "Bytes sequence data is NULL", NULL);
    infra_byte_sequence seq = infra_byte_sequence_init(bytes->capacity, NULL, 0);
    for (size_t i = 0; i < bytes->length; i++) {
        seq->data[i] = infra_byte_to_uppercase(bytes->data[i]);
    }
    return seq;
}

bool infra_byte_sequence_equal_ignore_case(infra_byte_sequence a, infra_byte_sequence b) {
    ASSERT(!a, "a is NULL", false);
    ASSERT(!b, "b is NULL", false);

    infra_byte_sequence c = infra_byte_sequence_to_lowercase(a);
    infra_byte_sequence d = infra_byte_sequence_to_lowercase(b);

    size_t size = c->length;
    if (d->length < size) {
        size = d->length;
    }

    bool res = strncmp((char*)c->data, (char*)d->data, size) == 0;
    infra_byte_sequence_destroy(&c);
    infra_byte_sequence_destroy(&d);
    return res;
}

infra_byte_sequence infra_byte_sequence_init(const size_t size, const char* data, const size_t length) {
    infra_byte_sequence bs = NULL;
    ALLOCATE(bs, sizeof(infra_byte_sequence));
    ASSERT(!bs, "infra_byte_sequence struct malloc failed", NULL);

    size_t init_length = size;
    if (init_length < INFRA_BYTE_SEQUENCE_DEFAULT_SIZE) {
        init_length = INFRA_BYTE_SEQUENCE_DEFAULT_SIZE;
    }
    ALLOCATE(bs->data, sizeof(char) * init_length);
    ASSERT(!bs->data, "infra_byte_sequence data malloc failed", NULL);


    bs->length= 0;
    bs->capacity= init_length;

    if (data) {
        infra_byte_sequence_grow_to(&bs, length);
        strncpy((char*)bs->data, data, length);
        bs->length += length;
    }

    return bs;
}

/*
 * frees memory for a string_buffer if it exists
 */
void infra_byte_sequence_destroy(infra_byte_sequence* ptr) {
    if (*ptr) {
        if ((*ptr)->data) {
            FREE((*ptr)->data);
        }
        FREE((*ptr));
    }
}

void infra_byte_sequence_grow(infra_byte_sequence* ptr) {
    ASSERT(!ptr, "NULL pointer passed as ptr", );
    ASSERT(!(*ptr), "NULL pointer passed for ptr's struct", );

    if ((*ptr)->capacity < INFRA_BYTE_SEQUENCE_DEFAULT_SIZE) {
        (*ptr)->capacity = INFRA_BYTE_SEQUENCE_DEFAULT_SIZE;
    }

    (*ptr)->capacity *= INFRA_BYTE_SEQUENCE_GROWTH_RATE;

    ALLOCATE(*ptr, (*ptr)->capacity);
}

void infra_byte_sequence_grow_to(infra_byte_sequence* ptr, size_t required_size) {
    ASSERT(!ptr, "NULL pointer passed as ptr", );
    ASSERT(!(*ptr), "NULL pointer passed for ptr's struct", );

    if ((*ptr)->capacity < INFRA_BYTE_SEQUENCE_DEFAULT_SIZE) {
        (*ptr)->capacity = INFRA_BYTE_SEQUENCE_DEFAULT_SIZE;
    }

    size_t newsize = (*ptr)->capacity;
    while (newsize <= required_size) {
        newsize *= INFRA_BYTE_SEQUENCE_GROWTH_RATE;
    }
    if (newsize != (*ptr)->capacity) {
        ALLOCATE(((*ptr)->data), newsize);
        (*ptr)->capacity = newsize;
    }
}

void infra_byte_sequence_append_raw(infra_byte_sequence* ptr, const char* data, const size_t strlength) {
    ASSERT(!ptr, "null pointer given as ptr", );
    ASSERT(!(*ptr), "null pointer given as ptr struct", );
    ASSERT(!data, "null pointer given as data", );

    size_t required_size = (*ptr)->length + strlength ;
    infra_byte_sequence_grow_to(ptr, required_size);

    strncat((char *)(*ptr)->data, data, strlength);
    (*ptr)->length += strlength;
}

void infra_byte_sequence_push_back(infra_byte_sequence* ptr, const char c) {
    ASSERT(!ptr, "null pointer given as ptr", );
    ASSERT(!(*ptr), "null pointer given as ptr struct", );

    if ((*ptr)->capacity <= (*ptr)->length) {
        infra_byte_sequence_grow(ptr);
    }

    (*ptr)->data[(*ptr)->length] = c;
    (*ptr)->length += 1;
}

void infra_byte_sequence_push_front(infra_byte_sequence* ptr, const char c) {
    if ((*ptr)->capacity <= (*ptr)->length) {
        infra_byte_sequence_grow(ptr);
    }

    for (size_t i = (*ptr)->length; i > 1; i++) {
        (*ptr)->data[i] = (*ptr)->data[i-1];
    }

    (*ptr)->data[0] = c;
    (*ptr)->length += 1;
}

/*
 *  append chunk_size data from f to ptr 
 *
 *   
 *  return: the output from fread() or 0 if the file can't be read from 
 */
size_t infra_byte_sequence_append_chunk(infra_byte_sequence* ptr, const size_t chunk_size, FILE* f) {
    ASSERT(!f, "FILE* f is NULL", 0);
    ASSERT(!ptr, "ptr is NULL", 0);
    ASSERT(!(*ptr), "ptr struct is NULL", 0);
    ASSERT(!(*ptr)->data, "ptr data is NULL", 0);

    size_t required_size = (*ptr)->length + chunk_size;
    infra_byte_sequence_grow_to(ptr, required_size);

    size_t res = fread((*ptr)->data + (*ptr)->length, 1, chunk_size, f);
    (*ptr)->length += res; //this only works because size is 1 (chars)
    return res;
}

infra_byte infra_byte_sequence_pop_front(infra_byte_sequence* ptr) {
    ASSERT(!ptr, "ptr is NULL", 0);
    ASSERT(!(*ptr), "ptr struct is NULL", 0);
    ASSERT(!(*ptr)->data, "ptr data is NULL", 0);

    infra_byte c = vector_get(*ptr, 0);
    (*ptr)->length -= 1;
    memmove((*ptr)->data, (*ptr)->data + 1, (*ptr)->length);

    return c;
}

