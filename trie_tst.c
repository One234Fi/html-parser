#include "arena.h"
#include "trie.h"
#include <assert.h>
#include <stdio.h>

int main() {
    trie t = {0};
    arena a = arena_init(4096);
    trie_prepare(&t, String("hello world"), &a);
    trie_prepare(&t, String("here's another string"), &a);
    trie_prepare(&t, String("different string"), &a);
    trie_prepare(&t, String("james"), &a);
    trie_prepare(&t, String("nyoom"), &a);
    trie_prepare(&t, String("ny"), &a);

    assert(trie_contains(t, String("hello world")) && "TRIE: hello world should be found");
    assert(!trie_contains(t, String("hello")) && "TRIE: hello should not be found");
    assert(!trie_contains(t, String("nyooom")) && "TRIE: nyooom should not be found");
    assert(trie_contains(t, String("ny")) && "TRIE: ny should be found");
    assert(!trie_contains(t, String("nyo")) && "TRIE: nyo should not be found");
    printf("TRIE: All tests passed\n");
}
