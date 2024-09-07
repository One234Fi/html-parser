#include "common.h"
#include <stddef.h>
#include <stdio.h>

#define NODE_STACK_DEFAULT_SIZE 8
#define NODE_STACK_GROWTH_RATE 2

typedef struct node {
    char type;
} node;

typedef struct node_stack {
    node* data;
    size_t size;
    size_t capacity;
    node* adjusted_current_node;
} node_stack;


node_stack* node_stack_init(const size_t size) {
    node_stack* ns;
    ALLOCATE(ns, size);
    if (!ns) {
        fprintf(stderr, "node_stack_init(): \"struct malloc failed\"\n");
        return NULL;
    }

    ALLOCATE(ns->data, sizeof(node) * size);
    if (!ns->data) {
        fprintf(stderr, "node_stack_init(): \"data malloc failed\"\n");
        return NULL;
    }

    ns->size = 0;
    ns->capacity = size;
    ns->adjusted_current_node = NULL;

    return ns;
}

void node_stack_destroy(node_stack* ns) {
    if (ns) {
        if (ns->data) {
            free(ns->data);
        }
        free(ns);
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
    if ((*ns)->size > 0) {
        (*ns)->size -= 1;
        return (*ns)->data[(*ns)->size];
    } else {
        fprintf(stderr, "node_stack_pop(): \"NODE STACK IS EMPTY\"");
        return (node){};
    }
}

node node_stack_peek(const node_stack* ns) {
    if (ns->size > 0) {
        return ns->data[ns->size-1];
    } else {
        fprintf(stderr, "node_stack_peek(): \"NODE STACK IS EMPTY\"");
        return (node){};
    }
}

node node_init(char type) {
    node n = {};
    n.type = type;
    return n;
}


