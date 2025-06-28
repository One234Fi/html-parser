#include "str.h"
#include <stdio.h>

// make the same comparisions as trie_tst.c with plain if statements for a rough
// estimate of performance scaling.

int main() {
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
        if (s_equal(strings[i % string_count], String("hello world"))
                || s_equal(strings[i % string_count], String("here's another string"))
                || s_equal(strings[i % string_count], String("different string"))
                || s_equal(strings[i % string_count], String("different string again"))
                || s_equal(strings[i % string_count], String("differenter string"))
                || s_equal(strings[i % string_count], String("james"))
                || s_equal(strings[i % string_count], String("john"))
                || s_equal(strings[i % string_count], String("peter"))
                || s_equal(strings[i % string_count], String("jared"))
                || s_equal(strings[i % string_count], String("jar head"))
                || s_equal(strings[i % string_count], String("jar tail"))
                || s_equal(strings[i % string_count], String("jartholomeu"))
                || s_equal(strings[i % string_count], String("nyoom"))
                || s_equal(strings[i % string_count], String("ny"))
            ) {
            matches++;
        }
    }

    printf("TRIE COMP: Finished\n");
    printf("%ld\n", matches);
}
