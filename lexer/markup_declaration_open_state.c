#include "lexer_internal.h"
#include "lexer.h"


void markup_declaration_open_state(lexer * p) {
    char backing[8];
    arena scratch = arena_wrap(8, backing);
    string buf = input_system_peekn(&p->input, 7, &scratch);

    if (buf.data[0] == '-' && buf.data[1] == '-') {
        input_system_consume(&p->input);
        input_system_consume(&p->input);
        set_current_token(p, token_comment_init());
        p->state = COMMENT_START_STATE;
        return;
    } 

    if (buf.len == 7) {
        if (strncmp(buf.data, "[CDATA[", buf.len) == 0) {
            for (uint32_t i = 0; i < buf.len; i++) {
                input_system_consume(&p->input);
            }

            if (adjusted_current_node() && !in_html_namespace_placeholder()) {
                p->state = CDATA_SECTION_STATE;
            } else {
                LOG_ERROR(xstr(CDATA_IN_HTML_CONTENT_PARSE_ERROR));
                set_current_token(p, token_comment_init());
                append_to_current_tag_token_comment_data(p, '[');
                append_to_current_tag_token_comment_data(p, 'C');
                append_to_current_tag_token_comment_data(p, 'D');
                append_to_current_tag_token_comment_data(p, 'A');
                append_to_current_tag_token_comment_data(p, 'T');
                append_to_current_tag_token_comment_data(p, 'A');
                append_to_current_tag_token_comment_data(p, '[');
                p->state = BOGUS_COMMENT_STATE;
            }
            return;
        }

        char lowercase_buf[buf.len] = {};
        for (uint32_t i = 0; i < buf.len; i++) {
            lowercase_buf[i] = tolower(buf.data[i]);
        }
        if (strncmp(lowercase_buf, "doctype", buf.len) == 0) {
            for (uint32_t i = 0; i < buf.len; i++) {
                input_system_consume(&p->input);
            }
            p->state = DOCTYPE_STATE;
            return;
        }
    }

    LOG_ERROR(xstr(INCORRECTLY_OPENED_COMMENT_PARSE_ERROR));
    set_current_token(p, token_comment_init());
    p->state = BOGUS_COMMENT_STATE;
}
