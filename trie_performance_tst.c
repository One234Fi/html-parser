#include "arena.h"
#include "trie.h"
#include <assert.h>
#include <stdio.h>

//This takes about 1.68 real time, while if statements take about 1.82.
//Not really impactful enough to use this by default, since it takes more
//memory and needs to be pre-baked and stored somewhere. Creating the trie
//inside the loop takes dramatically longer.
//
//TODO: investigate compile-time tries

int main() {
    trie t = {0};
    arena a = arena_init(4096 * 2);
    trie_prepare(&t, String("hello world"), &a);
    trie_prepare(&t, String("here's another string"), &a);
    trie_prepare(&t, String("different string"), &a);
    trie_prepare(&t, String("different string again"), &a);
    trie_prepare(&t, String("differenter string"), &a);
    trie_prepare(&t, String("james"), &a);
    trie_prepare(&t, String("john"), &a);
    trie_prepare(&t, String("peter"), &a);
    trie_prepare(&t, String("jared"), &a);
    trie_prepare(&t, String("jar head"), &a);
    trie_prepare(&t, String("jar tail"), &a);
    trie_prepare(&t, String("jartholomeu"), &a);
    trie_prepare(&t, String("nyoom"), &a);
    trie_prepare(&t, String("ny"), &a);


    string strings[] = {
        String("hello world"),
        String("hello"),
        String("nyooom"),
        String("ny"),
        String("nyo"),
        String("ASDF"),
        String("dif string"),
        String("diferenter  string"),
        String("differenter string"),
        String("differenter string "),
        String("james"),
        String("james the fifth"),
        String("john "),
        String("john"),
        String("jar man"),
        String("jar tail"),
        String("nyyyyy"),
        String("goodbye"),
        String("peta"),
        String("different string but again"),
        String("different string bad case"),
        String("different string long almost matches"),
        String("different string maybe this is enough"),
    };
    size string_count = sizeof(strings) / sizeof(string);
    size matches = 0;

    for (size i = 0; i < string_count * 1000000; i++) {
        if (trie_contains(t, strings[i % string_count])) {
            matches++;
        }
    }

    printf("TRIE PERF: Finished\n");
    printf("%ld\n", matches);
}

