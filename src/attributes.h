/*
 * Html attribute type definitions
 * 08/23/2024
 */

#ifndef HTML_ATTRIBUTES_MODULE
#define HTML_ATTRIBUTES_MODULE

enum HTML_ATTR_TYPE {
    HTML_ATTR_ACCEPT,
    HTML_ATTR_AUTOCOMPLETE,
    HTML_ATTR_CAPTURE,
    HTML_ATTR_CROSSORIGIN,
    HTML_ATTR_DIRNAME,
    HTML_ATTR_DISABLED,
    HTML_ATTR_ELEMENTTIMING,
    HTML_ATTR_FOR,
    HTML_ATTR_MAX,
    HTML_ATTR_MAXLENGTH,
    HTML_ATTR_MIN,
    HTML_ATTR_MINLENGTH,
    HTML_ATTR_MULTIPLE,
    HTML_ATTR_PATTERN,
    HTML_ATTR_PLACEHOLDER,
    HTML_ATTR_READONLY,
    HTML_ATTR_REL,
    HTML_ATTR_REQUIRED,
    HTML_ATTR_SIZE,
    HTML_ATTR_STEP,

    HTML_ATTR_COUNT_
};


enum HTML_GLOBAL_ATTR_TYPE {
    HTML_GLOBAL_ATTR_ACCESSKEY,
    HTML_GLOBAL_ATTR_ANCHOR,
    HTML_GLOBAL_ATTR_AUTOCAPITALIZE,
    HTML_GLOBAL_ATTR_AUTOFOCUS,
    HTML_GLOBAL_ATTR_CLASS,
    HTML_GLOBAL_ATTR_CONTENTEDITABLE,
    HTML_GLOBAL_ATTR_DATA,              //this one allows arbitrary data...
    HTML_GLOBAL_ATTR_DIR,
    HTML_GLOBAL_ATTR_DRAGGABLE,
    HTML_GLOBAL_ATTR_ENTERKEYHINT,
    HTML_GLOBAL_ATTR_EXPORTPARTS,
    HTML_GLOBAL_ATTR_HIDDEN,
    HTML_GLOBAL_ATTR_ID,
    HTML_GLOBAL_ATTR_INERT,
    HTML_GLOBAL_ATTR_INPUTMODE,
    HTML_GLOBAL_ATTR_IS,
    HTML_GLOBAL_ATTR_ITEMID,
    HTML_GLOBAL_ATTR_ITEMPROP,
    HTML_GLOBAL_ATTR_ITEMREF,
    HTML_GLOBAL_ATTR_ITEMSCOPE,
    HTML_GLOBAL_ATTR_ITEMTYPE,
    HTML_GLOBAL_ATTR_LANG,
    HTML_GLOBAL_ATTR_NONCE,
    HTML_GLOBAL_ATTR_PART,
    HTML_GLOBAL_ATTR_POPOVER,
    HTML_GLOBAL_ATTR_SLOT,
    HTML_GLOBAL_ATTR_SPELLCHECK,
    HTML_GLOBAL_ATTR_STYLE,
    HTML_GLOBAL_ATTR_TABINDEX,
    HTML_GLOBAL_ATTR_TITLE,
    HTML_GLOBAL_ATTR_TRANSLATE,
    HTML_GLOBAL_ATTR_VIRTUALKEYBOARDPOLICY,
    HTML_GLOBAL_ATTR_WRITINGSUGGESTIONS,

    HTML_GLOBAL_ATTR_COUNT_
};


#endif
