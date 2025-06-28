#ifndef TRIE_MODULE_H
#define TRIE_MODULE_H

#include "str.h"

typedef struct trie trie;
typedef struct {
    trie * data;
    size len;
    size cap;
} tries;

struct trie {
    string layer;
    tries routes;
};

void trie_prepare(trie * t, string s, arena * a);
bool trie_contains(trie t, string s);
#endif
