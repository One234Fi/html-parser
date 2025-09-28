#include "fickit.h"
#include "lexer.h"


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
    string name;
    string href;
    nodes children;
};

struct node_stack {
    node ** data;
    size len;
    size cap;
};

int print_content(node ** n, void * unused) {
    printf("%.*s", (int) (*n)->content.len, (*n)->content.data);
    return 1;
}

int print_link(node **n, void * unused) {
    if ((*n)->href.len > 0) {
        printf("%.*s\n", (int) (*n)->href.len, (*n)->href.data);
    }
    return 1;
}

int tree_for_each(node ** root, int (* callback)(node ** n, void * arg), void * arg) {
    if (root == NULL || *root == NULL) {
        fprintf(stderr, "NULL ROOT");
        return 1;
    }
    void * stack_buf = malloc(sizeof(node *) * 512);
    arena scratch = arena_wrap(sizeof(node *) * 512, stack_buf);
    scratch.debug_name = "tree scratch";
    node_stack stack = {0};
    *push(&stack, &scratch) = *root;

    while (stack.len > 0) {
        node * curr = stack.data[--stack.len];
        for (size i = 0; i < curr->children.len; i++) {
            *push(&stack, &scratch) = curr->children.data[i];
        }
        if(!callback(&curr, arg)) {
            free(stack_buf);
            return 0;
        }
    }

    free(stack_buf);
    return 1;
}

node * parse_tree(lexer * l, arena * node_pool, arena stack_buf, arena str_buf) {
    node * root = new(node_pool, node);
    node_stack open_elements = {0};
    string_buffer content_builder = {0};

    *push(&open_elements, &stack_buf) = root;

    do {
        token t = get_token(l);

        switch (t.type) {
            case DOCTYPE:
                //noop
                break;

            case START_TAG:
                if (t.tag.self_closing) {
                    node * n = new(node_pool, node);
                    n->name = *opt_unwrap(&t.tag.name, string, &String(""));
                    for (size i = 0; i < t.attrs.len; i++) {
                        if (s_equal_c(t.attrs.data[i].name, "href")) {
                            n->href = t.attrs.data[i].value;
                            break;
                        }
                    }
                    node * top = open_elements.data[open_elements.len-1];
                    *push(&top->children, node_pool) = n;
                } else {
                    node * n = new(node_pool, node);
                    *push(&open_elements, &stack_buf) = n;
                    n->name = *opt_unwrap(&t.tag.name, string, &String(""));
                    for (size i = 0; i < t.attrs.len; i++) {
                        if (s_equal_c(t.attrs.data[i].name, "href")) {
                            n->href = t.attrs.data[i].value;
                            break;
                        }
                    }
                    fprintf(stderr, "start->name: %.*s\n", (int) n->name.len, n->name.data);
                    fprintf(stderr, "pushing %.*s\n", (int)((string *) t.tag.name.val)->len, ((string *) t.tag.name.val)->data);
                }
                break;

            case END_TAG:
                node * pos = open_elements.data[open_elements.len - 2];
                node * top = open_elements.data[--open_elements.len];

                fprintf(stderr, "pos->name: %.*s\n", (int) pos->name.len, pos->name.data);
                fprintf(stderr, "top->name: %.*s\n", (int) top->name.len, top->name.data);

                fprintf(stderr, "%.*s", (int) content_builder.len, content_builder.data);
                top->content = s_clone((string){content_builder.data, content_builder.len, 0}, node_pool);
                content_builder.len = 0;

                *push(&pos->children, node_pool) = top;
                fprintf(stderr, "popping %.*s\n", (int)((string *) t.tag.name.val)->len, ((string *) t.tag.name.val)->data);
                break;

            case COMMENT:
                //noop
                break;

            case CHARACTER:
                *push(&content_builder, &str_buf) = t.character.data;
                break;

            case END_OF_FILE:
                //noop
                break;

            default:
        }

    } while (!l->eof_emitted);


    while (open_elements.len > 1) {
        *push(&root->children, node_pool) = open_elements.data[--open_elements.len];
        fprintf(stderr, "PUSHING UNFINISHED ELEM: %.*s\n", 
                (int) open_elements.data[open_elements.len]->name.len,
                open_elements.data[open_elements.len]->name.data);
    }

    return root;
}

