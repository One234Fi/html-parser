#ifndef CODE_POINT_TYPES_MODULE
#define CODE_POINT_TYPES_MODULE
//TODO: test this rigorously. recursive macros

/**
 * Code point utils and definitions
 *
 * Code points (unicode characters) follow the format:
 *      U+XXXX
 *  Where U+ is the unicode prefix and 'X' is a hexadecimal digit
 *  
 *  In this project, unicode character literals use 0x as their prefix.
 *  U+ is also a spec-conformat prefix that may be seen in strings 
 *  and comments
 */

#define is_leading_surrogate(c) (0xD800 <= c && c <= 0xDBFF)
#define is_trailing_surrogate(c) (0xDC00 <= c && c <= 0xDFFF)
#define is_surrogate(c) (is_leading_surrogate(c) || is_trailing_surrogate(c))
#define is_scalar_value(c) (!is_surrogate(c))
#define is_non_char(c) (                \
        (0xFDD0 <= c && c <= 0xFDEF)    \
        || c == 0xFFFE                  \
        || c == 0xFFFF                  \
        || c == 0x1FFFE                 \
        || c == 0x1FFFF                 \
        || c == 0x2FFFE                 \
        || c == 0x2FFFF                 \
        || c == 0x3FFFE                 \
        || c == 0x3FFFF                 \
        || c == 0x4FFFE                 \
        || c == 0x4FFFF                 \
        || c == 0x5FFFE                 \
        || c == 0x5FFFF                 \
        || c == 0x6FFFE                 \
        || c == 0x6FFFF                 \
        || c == 0x7FFFE                 \
        || c == 0x7FFFF                 \
        || c == 0x8FFFE                 \
        || c == 0x8FFFF                 \
        || c == 0x9FFFE                 \
        || c == 0x9FFFF                 \
        || c == 0xAFFFE                 \
        || c == 0xAFFFF                 \
        || c == 0xBFFFE                 \
        || c == 0xBFFFF                 \
        || c == 0xCFFFE                 \
        || c == 0xCFFFF                 \
        || c == 0xDFFFE                 \
        || c == 0xDFFFF                 \
        || c == 0xEFFFE                 \
        || c == 0xEFFFF                 \
        || c == 0xFFFFE                 \
        || c == 0xFFFFF                 \
        || c == 0x10FFFE                \
        || c == 0x10FFFF                \
        )
#define is_ascii_code_point(c) (0x0000 <= c && c <= 0x007F)
#define is_ascii_tab_or_newline(c) (c == 0x0009 || c == 0x000A || c == 0x000D)
#define is_ascii_whitespace(c) (is_ascii_tab_or_newline(c) || c == 0x000C || c == 0x0020)
#define is_czero_ctrl(c) (c >= 0x0000 && c <= 0x001F) 
#define is_czero_ctrl_or_space(c) (is_czero_ctrl(c) || c == 0x0020)
#define is_control(c) (is_czero_ctrl(c) || (0x007F <= c && c <= 0x009F))
#define is_ascii_digit(c) (0x0030 <= c && c <= 0x0039)
#define is_ascii_upper_hex_digit(c) (is_ascii_digit(c) || (0x0041 <= c && c <= 0x0046))
#define is_ascii_lower_hex_digit(c) (is_ascii_digit(c) || (0x0061 <= c && c <= 0x0066))
#define is_ascii_hex_digit(c) (is_ascii_upper_hex_digit(c) || is_ascii_lower_hex_digit(c))
#define is_ascii_upper_alpha(c) (0x0041 <= c && c <= 0x005A)
#define is_ascii_lower_alpha(c) (0x0061 <= c && c <= 0x007A)
#define is_ascii_alpha(c) (is_ascii_upper_alpha(c) || is_ascii_lower_alpha(c))
#define is_ascii_alphanumeric(c) (is_ascii_digit(c) || is_ascii_alpha(c))

#endif
