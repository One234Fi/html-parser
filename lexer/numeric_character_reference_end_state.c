#include "lexer_internal.h"
#include "lexer.h"


void numeric_character_reference_end_state(lexer * p) {
    int char_ref = p->char_ref_code;
    if (char_ref == 0x00) {
        LOG_ERROR(xstr(NULL_CHARACTER_REFERENCE_PARSE_ERROR));
        p->char_ref_code = 0xFFFD;
    } else if (char_ref > 0x10FFFF) {
        LOG_ERROR(xstr(CHARACTER_REFERENCE_OUTSIDE_OF_UNICODE_RANGE_PARSE_ERROR));
        p->char_ref_code = 0xFFFD;
    } else if (is_surrogate(char_ref)) {
        LOG_ERROR(xstr(SURROGATE_CHARACTER_REFERENCE_PARSE_ERROR));
        p->char_ref_code = 0xFFFD;
    } else if (is_non_char(char_ref)) {
        LOG_ERROR(xstr(NONCHARACTER_CHARACTER_REFERENCE_PARSE_ERROR));
    } else if (char_ref == 0x0D 
            || (is_control(char_ref) && !is_ascii_whitespace(char_ref))) {
        LOG_ERROR(xstr(CONTROL_CHARACTER_REFERENCE_PARSE_ERROR));
    } else {
#define map_case(num, code_point) case num: p->char_ref_code = code_point; break
        switch (char_ref) {
            map_case(0x80, 0x20AC);
            map_case(0x82, 0x201A);
            map_case(0x83, 0x0192);
            map_case(0x84, 0x201E);
            map_case(0x85, 0x2026);
            map_case(0x86, 0x2020);
            map_case(0x87, 0x2021);
            map_case(0x88, 0x02C6);
            map_case(0x89, 0x2030);
            map_case(0x8A, 0x0160);
            map_case(0x8B, 0x2039);
            map_case(0x8C, 0x0152);
            map_case(0x8E, 0x017D);
            map_case(0x91, 0x2018);
            map_case(0x92, 0x2019);
            map_case(0x93, 0x201C);
            map_case(0x94, 0x201D);
            map_case(0x95, 0x2022);
            map_case(0x96, 0x2013);
            map_case(0x97, 0x2014);
            map_case(0x98, 0x02DC);
            map_case(0x99, 0x2122);
            map_case(0x9A, 0x0161);
            map_case(0x9B, 0x203A);
            map_case(0x9C, 0x0153);
            map_case(0x9E, 0x017E);
            map_case(0x9F, 0x0178);
        }
#undef map_case
        clear_temporary_buffer(p);
        append_to_temp_buffer(p, char_ref);
        flush_code_points(p);
        p->state = p->return_state;
    }
}
