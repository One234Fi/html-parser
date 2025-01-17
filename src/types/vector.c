#include "types/arena.h"
#include "types/types.h"
#include <assert.h>
#include <string.h>


void grow(void * slice, size stride, arena * a) {
    struct {
        void * data;
        size len;
        size cap;
    } temp;
    memcpy(&temp, slice, sizeof(temp));
    assert(temp.len >= 0);
    assert(temp.cap >= 0);
    assert(temp.len <= temp.cap);

    size align = 16;
    if (!temp.data) {
        temp.cap = 1;
        temp.data = alloc(a, stride * 2, align, temp.cap);
    }
    else if (a->beg == temp.data + stride * temp.cap) {
        alloc(a, stride, 1, temp.cap); 
    }
    else {
        void * data = alloc(a, stride * 2, align, temp.cap);
        memcpy(data, temp.data, stride * temp.len);
        temp.data = data;
    }

    temp.cap *= 2;
    memcpy(slice, &temp, sizeof(temp));
}
