typedef struct node node;
typedef struct nodes nodes;
typedef struct node_stack node_stack;

struct nodes {
    node ** data;
    size len;
    size cap;
};

struct node {
    string content;
    nodes children;
};

struct node_stack {
    node ** data;
    size len;
    size cap;
};

node * parse_tree(lexer * l, arena * node_pool, arena stack_buf, arena str_buf);
int print_content(node ** n, void * unused);
int tree_for_each(node ** root, int (* callback)(node ** n, void * arg), void * arg);
