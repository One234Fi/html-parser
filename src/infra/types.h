#ifndef INFRA_TYPES_MODULE
#define INFRA_TYPES_MODULE

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

// BYTES and BYTE SEQUENCES

typedef unsigned char infra_byte;
typedef struct infra_byte_sequence {
    infra_byte * data;
    size_t length;
    size_t capacity;
} * infra_byte_sequence;

//spec defined
infra_byte infra_byte_to_lowercase(infra_byte byte);
infra_byte infra_byte_to_uppercase(infra_byte byte);

infra_byte_sequence infra_byte_sequenceto_lowercase(infra_byte_sequence bytes);
infra_byte_sequence infra_byte_sequenceto_uppercase(infra_byte_sequence bytes);
bool infra_byte_sequence_equal_ignore_case(infra_byte_sequence a, infra_byte_sequence b);

bool infra_byte_sequence_is_potential_prefix(infra_byte_sequence potential_prefix, infra_byte_sequence input);

//impl details
infra_byte_sequence infra_byte_sequence_init(const size_t size, const char* data, const size_t length);
void infra_byte_sequence_destroy(infra_byte_sequence* ptr);
void infra_byte_sequence_grow_to(infra_byte_sequence* ptr, size_t required_size);
void infra_byte_sequence_append_raw(infra_byte_sequence* ptr, const char* data, const size_t strlength);
size_t infra_byte_sequence_append_chunk(infra_byte_sequence* ptr, const size_t chunk_size, FILE* f);
void infra_byte_sequence_push_back(infra_byte_sequence* ptr, const char c);
void infra_byte_sequence_push_front(infra_byte_sequence* ptr, const char c);
infra_byte infra_byte_sequence_pop_front(infra_byte_sequence* ptr);

// End of BYTES and BYTE SEQUENCES


#endif
