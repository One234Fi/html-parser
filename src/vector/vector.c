#include <string.h>
#include "vector.h"


void grow(void * slice, ptrdiff_t size, ptrdiff_t align, arena * a) {
    struct {
        void * data;
        ptrdiff_t len;
        ptrdiff_t cap;
    } temp;
    memcpy(&temp, slice, sizeof(temp));
    assert(temp.len >= 0);
    assert(temp.cap >= 0);
    //assert(temp.len <= temp.cap);

    if (!temp.data) {
        temp.cap = 1;
        temp.data = alloc(a, size * 2, align, temp.cap);
    }
    else if (a->beg == temp.data + size * temp.cap) {
        alloc(a, size, 1, temp.cap); 
    }
    else {
        void * data = alloc(a, size * 2, align, temp.cap);
        memcpy(data, temp.data, size * temp.len);
        temp.data = data;
    }

    temp.cap *= 2;
    memcpy(slice, &temp, sizeof(temp));
}
