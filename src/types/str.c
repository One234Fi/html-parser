#include "types/str.h"
#include "mem/arena.h"
#include <string.h>
#include "common.h"

bool s_equal(string a, string b) {
    return a.len == b.len && (strncmp(a.data, b.data, a.len) == 0);
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
