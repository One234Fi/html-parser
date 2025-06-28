#ifndef TRIE_MODULE_H
#define TRIE_MODULE_H

#include "types.h"
#include "str.h"

typedef struct trie trie;
struct trie {
    struct trie_path {
        char c;
        trie * path;
    } * data;
    size len;
    size cap;
};

void trie_prepare(trie * t, string s, arena * a);
bool trie_contains(trie t, string s);

#endif
