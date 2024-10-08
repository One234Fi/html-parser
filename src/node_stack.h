#ifndef NODE_STACK_MODULE
#define NODE_STACK_MODULE

#include <stdbool.h>
#include <stdlib.h>
#include "input.h"

#define NODE_STACK_DEFAULT_SIZE 8
#define NODE_STACK_GROWTH_RATE 2

typedef struct node {
    int type;
    string_buffer name;
} node;

typedef struct node_stack {
    node* data;
    size_t size;
    size_t capacity;
    node* adjusted_current_node;
} node_stack;


node_stack* node_stack_init(const size_t size);
void node_stack_destroy(node_stack** ns);
void node_stack_push(const node n, node_stack** ns);
node node_stack_pop(node_stack** ns);
node node_stack_peek(const node_stack* ns);
bool node_stack_is_empty(const node_stack* ns);

node node_init(char type);

#endif
