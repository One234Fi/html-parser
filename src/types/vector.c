#include "types/arena.h"
#include "types/types.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>


void grow(void * slice, size stride, size align, arena * a) {
    struct {
        void * data;
        size len;
        size cap;
    } temp;
    memcpy(&temp, slice, sizeof(temp));
    fprintf(stderr, "GROW START: %td, %td\n", temp.len, temp.cap);
    assert(temp.len >= 0);
    assert(temp.cap >= 0);
    assert(temp.len <= temp.cap);

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
    fprintf(stderr, "GROW END: %td, %td\n", temp.len, temp.cap);
    memcpy(slice, &temp, sizeof(temp));
}
