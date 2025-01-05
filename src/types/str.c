#include "types/str.h"
#include <string.h>

bool string_equal(string a, string b) {
    return a.len == b.len && (strncmp(a.data, b.data, a.len) == 0);
}
