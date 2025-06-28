#include "trie.h"
#include "vector.h"

size trie_has_(trie t, char c) {
    for (size i = 0; i < t.len; i++) {
        if (t.data[i].c == c) {
            return i;
        }
    }

    return -1;
}

void trie_prepare(trie * t, string s, arena * a) {
    for (size i = 0; i < s.len; i++) {
        size pos = trie_has_(*t, s.data[i]);
        if (pos != -1) {
            t = t->data[pos].path;
            continue;
        } else {
            *push(t, a) = (struct trie_path) {
                .c = s.data[i],
                .path = new(a, trie),
            };
            t = t->data[t->len-1].path;
        }
    }
    *push(t, a) = (struct trie_path) {
        .c = '\0',
        .path = NULL
    };
}

bool trie_contains(trie t, string s) {
    for (size i = 0; i < s.len; i++) {
        size pos = trie_has_(t, s.data[i]);
        if (pos == -1) {
            return false;
        }
        t = *t.data[pos].path;
    }
    if (trie_has_(t, '\0') != -1) {
        return true;
    }
    return false;
}
