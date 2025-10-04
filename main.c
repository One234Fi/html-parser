/*
 * Html parser entry point
 * 08/24/2024
 * 09/19/2025 - lol I thought this would be a two week project when I started...
 */


#include "lexer.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "fickit.h"


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

node * parse_tree(lexer * l, arena * node_pool, arena stack_buf, arena str_buf);
int print_content(node ** n, void * unused);
int print_link(node **n, void * unused);
int tree_for_each(node ** root, int (* callback)(node ** n, void * arg), void * arg);

void print_as_url(string href);



bool parse_links = false;
char * file_path = NULL;
char * doc_domain = NULL;

arena global;


int main(int argc, char* argv[]) {
    fprintf(stderr, "Passed %d arguments\n", argc);


    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--parse-links") == 0) {
            parse_links = true;
        } else if (!file_path) {
            file_path = argv[i];
        } else if (parse_links && file_path) {
            break;
        }
    }

    if (file_path) {
        char * backing = malloc(1 << 18);
        assert(backing != NULL);
        global = arena_wrap(1 << 18, backing);
        arena scratch1 = arena_slice(&global, 1 << 12);
        arena scratch2 = arena_slice(&global, 1 << 12);
        global.debug_name = "global";
        scratch1.debug_name = "stack mem";
        scratch2.debug_name = "string mem";

        lexer lex = lexer_init(file_path, &global);
        node * tree = parse_tree(&lex, &global, scratch1, scratch2);
        if (parse_links) {
            tree_for_each(&tree, print_link, NULL);
        } else {
            tree_for_each(&tree, print_content, NULL);
        }
        fprintf(stderr, "\nEOF\n");
        free(backing);
    }

    return EXIT_SUCCESS;
}

void print_as_url(string href) {
    if (!parse_links) return;
    if (href.len < 1) return;

    if (href.data[0] == '/' && doc_domain != NULL) {
        href = s_cat(String(doc_domain), href, &global);
        href = s_cat(String("http://"), href, &global);
    } else if (doc_domain != NULL) {
        //
    }

    printf("%.*s\n", (int) href.len, href.data);
}

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
                    n->name = t.tag.name;
                    for (size i = 0; i < t.attrs.len; i++) {
                        if (s_equal_c(t.attrs.data[i].name, "href")) {
                            n->href = t.attrs.data[i].value;
                            print_as_url(n->href);
                            break;
                        }
                    }
                    node * top = open_elements.data[open_elements.len-1];
                    *push(&top->children, node_pool) = n;
                } else {
                    node * n = new(node_pool, node);
                    *push(&open_elements, &stack_buf) = n;
                    n->name = t.tag.name;
                    for (size i = 0; i < t.attrs.len; i++) {
                        if (s_equal_c(t.attrs.data[i].name, "href")) {
                            n->href = t.attrs.data[i].value;
                            print_as_url(n->href);
                            break;
                        }
                    }
                    fprintf(stderr, "start->name: %.*s\n", (int) n->name.len, n->name.data);
                    fprintf(stderr, "pushing %.*s\n", (int)t.tag.name.len, t.tag.name.data);
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
                fprintf(stderr, "popping %.*s\n", (int)t.tag.name.len, t.tag.name.data);
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

