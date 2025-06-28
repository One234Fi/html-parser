#include "str.h"
#include "arena.h"
#include <ctype.h>
#include <string.h>

bool s_equal(string a, string b) {
    return a.len == b.len && (strncmp(a.data, b.data, a.len) == 0);
}

bool s_equal_ignore_case(string a, string b) {
    if (a.len != b.len) {
        return false;
    }

    for (size i = 0; i < a.len; i++) {
        if (tolower(a.data[i]) != tolower(b.data[i])) {
            return false;
        }
    }

    return true;
}

size s_has_char(string a, char c) {
    for (size i = 0; i < a.len; i++) {
        if (a.data[i] == c) {
            return i;
        }
    }
    return -1;
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
