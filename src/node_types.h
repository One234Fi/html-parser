#ifndef TREE_NODE_TYPES_MODULES
#define TREE_NODE_TYPES_MODULES

//TODO: figure out if this is the right place for this...
typedef enum NODE_TYPES {
    MATHML_MI_ELEMENT,
    MATHML_MO_ELEMENT,
    MATHML_MN_ELEMENT,
    MATHML_MS_ELEMENT,
    MATHML_MTEXT_ELEMENT,
    MATHML_ANNOTATION_XML_ELEMENT,
    SVG_FOREIGN_OBJECT_ELEMENT,
    SVG_DESC_ELEMENT,
    SVG_TITLE_ELEMENT,
} NODE_TYPES;

#define is_mathml_text_integration_point(node) \
        (node.type == MATHML_MI_ELEMENT \
        || node.type == MATHML_MO_ELEMENT \
        || node.type == MATHML_MN_ELEMENT \
        || node.type == MATHML_MS_ELEMENT \
        || node.type == MATHML_MTEXT_ELEMENT)

#endif 
