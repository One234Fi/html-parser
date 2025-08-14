#ifndef PARSER_H
#define PARSER_H

#include "fickit.h"
#include "lexer.h"

typedef enum {
    HTML_DOCUMENT,
    HTML_DOCTYPE,
    HTML_COMMENT,
    HTML_TEMPLATE,
    HTML_TABLE,
    HTML_TBODY,
    HTML_TFOOT,
    HTML_THEAD,
    HTML_TR,
} node_type;

typedef struct node node;
typedef struct nodes nodes;

struct nodes {
    node * data;
    size len;
    size cap;
};

typedef struct {
    string name;
    string value;
} node_attr;

typedef struct {
    node_attr * data;
    size len;
    size cap;
} node_attrs;

typedef struct {
    bool force_quirks;
} document_node_t;

typedef struct {
    string name;
    string public_id;
    string system_id;
} doctype_node_t;

typedef struct {
    string data;
} comment_node_t;

typedef struct {
    node * contents; //TODO: this is probably wrong
} template_node_t;

struct node {
    string name;
    node_attrs attributes;
    union {
        document_node_t document;
        doctype_node_t doctype;
        comment_node_t comment;
        template_node_t template;
    };
    node * parent;
    nodes children;
    node_type type;
};



typedef enum INSERTION_MODE_TYPE {
    INSERTION_MODE_INITIAL,
    BEFORE_HTML,
    BEFORE_HEAD,
    IN_HEAD,
    IN_HEAD_NOSCRIPT,
    AFTER_HEAD,
    IN_BODY,
    TEXT,
    IN_TABLE,
    IN_TABLE_TEXT, 
    IN_CAPTION, 
    IN_COLUMN_GROUP,
    IN_TABLE_BODY,
    IN_ROW,
    IN_CELL,
    IN_SELECT,
    IN_SELECT_IN_TABLE,
    IN_TEMPLATE,
    AFTER_BODY,
    IN_FRAMESET,
    AFTER_FRAMESET,
    AFTER_AFTER_BODY,
    AFTER_AFTER_FRAMESET
} insertion_mode;

typedef struct {
    arena * arena;
    nodes open_elem_stack;
    node document;
    enum INSERTION_MODE_TYPE insert_mode;
    enum INSERTION_MODE_TYPE original_insert_mode;
    bool frameset_ok;
    bool parser_mode_frozen;
    lexer lex;
} parser;

parser parser_init(const char* filename, arena * a);

#endif
