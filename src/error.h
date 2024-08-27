#ifndef ERROR_MODULE
#define ERROR_MODULE

/*
 * Constants and functions for error logging
 *
 */

#include "tokenizer.h"
enum HTML_PARSER_ERROR {
    //SPEC: Parse errors
    ABRUPT_CLOSING_OF_EMPTY_COMMENT_PARSE_ERROR,
    ABRUPT_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR,
    ABRUPT_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR,
    ABSENCE_OF_DIGITS_IN_NUMERIC_CHARACTER_REFERENCE_PARSE_ERROR,
    CDATA_IN_HTML_CONTENT_PARSE_ERROR,
    CHARACTER_REFERENCE_OUTSIDE_OF_UNICODE_RANGE_PARSE_ERROR,
    CONTROL_CHARACTER_IN_INPUT_STREAM_PARSE_ERROR,
    CONTROL_CHARACTER_REFERENCE_PARSE_ERROR,
    DUPLICATE_ATTRIBUTE_PARSE_ERROR,
    END_TAG_WITH_ATTRIBUTES_PARSE_ERROR,
    END_TAG_WITH_TRAILING_SOLIDUS_PARSE_ERROR,
    EOF_BEFORE_TAG_NAME_PARSE_ERROR,
    EOF_IN_CDATA_PARSE_ERROR,
    EOF_IN_COMMENT_PARSE_ERROR,
    EOF_IN_DOCTYPE_PARSE_ERROR,
    EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT_PARSE_ERROR,
    EOF_IN_TAG_PARSE_ERROR,
    INCORRECTLY_CLOSED_COMMENT_PARSE_ERROR,
    INCORRECTLY_OPENED_COMMENT_PARSE_ERROR,
    INVALID_CHARACTER_SEQUENCE_AFTER_DOCTYPE_NAME_PARSE_ERROR,
    INVALID_FIRST_CHARACTER_OF_TAG_NAME_PARSE_ERROR,
    MISSING_ATTRIBUTE_VALUE_PARSE_ERROR,
    MISSING_DOCTYPE_NAME_PARSE_ERROR,
    MISSING_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR,
    MISSING_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR,
    MISSING_END_TAG_NAME_PARSE_ERROR,
    MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER_PARSE_ERROR,
    MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR,
    MISSING_WHITESPACE_BEFORE_DOCTYPE_NAME_PARSE_ERROR,
    MISSING_WHITESPACE_BETWEEN_ATTRIBUTES_PARSE_ERROR,
    MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS_PARSE_ERROR,
    NESTED_COMMENT_PARSE_ERROR,
    NONCHARACTER_CHARACTER_REFERENCE_PARSE_ERROR,
    NONCHARACTER_IN_INPUT_STREAM_PARSE_ERROR,
    NON_VOID_HTML_ELEMENT_START_TAG_WITH_TRAILING_SOLIDUS_PARSE_ERROR,
    NULL_CHARACTER_REFERENCE_PARSE_ERROR,
    SURROGATE_CHARACTER_REFERENCE_PARSE_ERROR,
    SURROGATE_IN_INPUT_STREAM_PARSE_ERROR,
    UNEXPECTED_CHARACTER_AFTER_DOCTYPE_SYSTEM_IDENTIFIER_PARSE_ERROR,
    UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME_PARSE_ERROR,
    UNEXPECTED_CHARACTER_IN_UNQUOTED_ATTRIBUTE_VALUE_PARSE_ERROR,
    UNEXPECTED_EQUALS_SIGN_BEFORE_ATTRIBUTE_NAME_PARSE_ERROR,
    UNEXPECTED_NULL_CHARACTER_PARSE_ERROR,
    UNEXPECTED_QUESTION_MARK_INSTEAD_OF_TAG_NAME_PARSE_ERROR,
    UNEXPECTED_SOLIDUS_IN_TAG_PARSE_ERROR,
    UNKNOWN_NAMED_CHARACTER_REFERENCE_PARSE_ERROR,

    //NON-SPEC: internal errors
    INVALID_TOKENIZER_STATE_ERROR,
};

void log_error(enum HTML_PARSER_ERROR error);

#endif
