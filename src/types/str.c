#include "types/str.h"
#include "mem/arena.h"
#include <ctype.h>
#include <string.h>
#include "common.h"

bool s_equal(string a, string b) {
    return a.len == b.len && (strncmp(a.data, b.data, a.len) == 0);
}

bool s_equal_ignore_case(string a, string b, arena scratch) {
    a = s_clone(a, &scratch);
    b = s_clone(b, &scratch);
    return s_equal(a, b);
}

string s_clone(string s, arena * a) {
    string n = s;
    n.data = new(a, char, s.len);
    if (n.len > 0) {
        memcpy(n.data, s.data, n.len);
    }
    return n;
}

string s_cat(string a, string b, arena * perm) {
    string s = a;
    if (!s.data || s.data + s.len != perm->beg) {
        s = s_clone(a, perm);
    }
    s.len += s_clone(b, perm).len;
    return s;
}

//mutates input
string s_to_lower(string a) {
    for (size i = 0; i < a.len; i++) {
        a.data[i] = tolower(a.data[i]);
    }
    return a;
}

//mutates input
string s_to_upper(string a) {
    for (size i = 0; i < a.len; i++) {
        a.data[i] = toupper(a.data[i]);
    }
    return a;
}
