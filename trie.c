#include "trie.h"
#include "vector.h"

void trie_prepare(trie * t, string s, arena * a) {
    for (size i = 0; i < s.len; i++) {
        size pos = s_has_char(t->layer, s.data[i]);
        if (pos != -1) {
            t = &t->routes.data[pos];
            continue;
        } else {
            *push(&t->layer, a) = s.data[i];
            *push(&t->routes, a) = (trie){0};
            t = &t->routes.data[t->routes.len - 1];
        }
    }
    *push(&t->layer, a) = '\0'; //mark this as a valid terminal
    *push(&t->routes, a) = (trie){0};//placeholder
}

bool trie_contains(trie t, string s) {
    for (size i = 0; i < s.len; i++) {
        size pos = s_has_char(t.layer, s.data[i]);
        if (pos == -1) {
            return false;
        }
        t = t.routes.data[pos];
    }
    if (s_has_char(t.layer, '\0') != -1) {
        return true;
    }
    return false;
}
