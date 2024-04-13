translate_newlines(const char *s, int exec_input, struct tok_state *tok) {
    int skip_next_lf = 0;
    size_t needed_length = strlen(s) + 2, final_length;
    char *buf, *current;
    char c = '\0';
    buf = PyMem_MALLOC(needed_length);
    if (buf == NULL) {
        tok->done = E_NOMEM;
        return NULL;
    }
    for (current = buf; *s; s++, current++) {
        c = *s;
        if (skip_next_lf) {
            skip_next_lf = 0;
            if (c == '\n') {
                c = *++s;
                if (!c)
                    break;
            }
        }
        if (c == '\r') {
            skip_next_lf = 1;
            c = '\n';
        }
        *current = c;
    }
    /* If this is exec input, add a newline to the end of the string if
       there isn't one already. */
    if (exec_input && c != '\n') {
        *current = '\n';
        current++;
    }
    *current = '\0';
    final_length = current - buf + 1;
    if (final_length < needed_length && final_length)
        /* should never fail */
        buf = PyMem_REALLOC(buf, final_length);
    return buf;
}