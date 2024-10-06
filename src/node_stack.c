#include "common.h"
#include "node_stack.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>


node_stack* node_stack_init(const size_t size) {
    node_stack* ns = NULL;
    ALLOCATE(ns, size);
    ASSERT(!ns, "struct malloc failed", NULL); 

    ALLOCATE(ns->data, sizeof(node) * size);
    ASSERT(!ns->data, "data malloc failed", NULL); 

    ns->size = 0;
    ns->capacity = size;
    ns->adjusted_current_node = NULL;

    return ns;
}

void node_stack_destroy(node_stack** ns) {
    ASSERT(!ns, "Node stack pointer is NULL", );
    if ((*ns)) {
        if ((*ns)->data) {
            FREE((*ns)->data);
        }
        FREE(*ns);
    }
}


void node_stack_push(const node n, node_stack** ns) {
    if ((*ns)->size >= (*ns)->capacity) {
        size_t newsize = (*ns)->capacity * NODE_STACK_GROWTH_RATE;
        ALLOCATE((*ns)->data, newsize);
        (*ns)->capacity = newsize;
    }

    (*ns)->data[(*ns)->size] = n;
    (*ns)->size += 1;
}

node node_stack_pop(node_stack** ns) {
    ASSERT(!ns, "NULL pointer passed as ns", (node){});
    ASSERT(!(*ns), "Node stack struct is NULL", (node){});
    ASSERT(!(*ns)->data, "Node stack data is NULL", (node){});
    ASSERT(!node_stack_is_empty(*ns), "Node stack is empty", (node){});

    (*ns)->size -= 1;
    return (*ns)->data[(*ns)->size];
}

node node_stack_peek(const node_stack* ns) {
    ASSERT(!ns, "NULL pointer passed as ns", (node){});

    if (ns->size > 0) {
        return ns->data[ns->size-1];
    } else {
        LOG_ERROR("NODE STACK IS EMPTY");
        return (node){};
    }
}

bool node_stack_is_empty(const node_stack* ns) {
    ASSERT(!ns, "NULL pointer passed as ns", true);
    return ns->size == 0;
}

node node_init(char type) {
    node n = {};
    n.type = type;
    return n;
}


