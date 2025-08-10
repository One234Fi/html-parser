#include <stdio.h>
#include <assert.h>

#define FICKIT_IMPL
#include "fickit.h"

struct i32s {
    i32 * data;
    size len;
    size cap;
};

int main() {
    arena a = arena_init(256);
    struct i32s test_vec = {0};
    *push_front(&test_vec, &a) = 5;
    *push_front(&test_vec, &a) = 4;
    *push_front(&test_vec, &a) = 3;
    *push_front(&test_vec, &a) = 2;
    *push_front(&test_vec, &a) = 1;
    *push_front(&test_vec, &a) = 0;

    assert(test_vec.len == 6 && "VECTOR: 6 items should have been appended");
    assert(test_vec.cap == 8 && "VECTOR: The growth factor is 2");
    for (size i = 0; i < test_vec.len; i++) {
        assert(test_vec.data[i] == i && "VECTOR: The items should be in reverse order");
    }
    printf("VECTOR: Queuing works\n");


    size i = 0;
    while (test_vec.len > 0) {
        printf("%d\n", test_vec.data[0]);
        assert(test_vec.data[0] == i && "VECTOR: The items should be in reverse order");
        i++;
        pop_front(&test_vec);
    }
    assert(test_vec.len == 0 && "VECTOR: vector should be empty");
    printf("VECTOR: Popping works\n");


    printf("VECTOR: All vector tests passed\n");
}
