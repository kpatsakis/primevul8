check_bom(int get_char(struct tok_state *),
          void unget_char(int, struct tok_state *),
          int set_readline(struct tok_state *, const char *),
          struct tok_state *tok)
{
    int ch1, ch2, ch3;
    ch1 = get_char(tok);
    tok->decoding_state = STATE_RAW;
    if (ch1 == EOF) {
        return 1;
    } else if (ch1 == 0xEF) {
        ch2 = get_char(tok);
        if (ch2 != 0xBB) {
            unget_char(ch2, tok);
            unget_char(ch1, tok);
            return 1;
        }
        ch3 = get_char(tok);
        if (ch3 != 0xBF) {
            unget_char(ch3, tok);
            unget_char(ch2, tok);
            unget_char(ch1, tok);
            return 1;
        }
#if 0
    /* Disable support for UTF-16 BOMs until a decision
       is made whether this needs to be supported.  */
    } else if (ch1 == 0xFE) {
        ch2 = get_char(tok);
        if (ch2 != 0xFF) {
            unget_char(ch2, tok);
            unget_char(ch1, tok);
            return 1;
        }
        if (!set_readline(tok, "utf-16-be"))
            return 0;
        tok->decoding_state = STATE_NORMAL;
    } else if (ch1 == 0xFF) {
        ch2 = get_char(tok);
        if (ch2 != 0xFE) {
            unget_char(ch2, tok);
            unget_char(ch1, tok);
            return 1;
        }
        if (!set_readline(tok, "utf-16-le"))
            return 0;
        tok->decoding_state = STATE_NORMAL;
#endif
    } else {
        unget_char(ch1, tok);
        return 1;
    }
    if (tok->encoding != NULL)
        PyMem_FREE(tok->encoding);
    tok->encoding = new_string("utf-8", 5, tok);
    if (!tok->encoding)
        return 0;
    /* No need to set_readline: input is already utf-8 */
    return 1;
}