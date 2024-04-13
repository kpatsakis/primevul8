check_coding_spec(const char* line, Py_ssize_t size, struct tok_state *tok,
                  int set_readline(struct tok_state *, const char *))
{
    char *cs;
    int r = 1;

    if (tok->cont_line) {
        /* It's a continuation line, so it can't be a coding spec. */
        tok->read_coding_spec = 1;
        return 1;
    }
    if (!get_coding_spec(line, &cs, size, tok))
        return 0;
    if (!cs) {
        Py_ssize_t i;
        for (i = 0; i < size; i++) {
            if (line[i] == '#' || line[i] == '\n' || line[i] == '\r')
                break;
            if (line[i] != ' ' && line[i] != '\t' && line[i] != '\014') {
                /* Stop checking coding spec after a line containing
                 * anything except a comment. */
                tok->read_coding_spec = 1;
                break;
            }
        }
        return 1;
    }
    tok->read_coding_spec = 1;
    if (tok->encoding == NULL) {
        assert(tok->decoding_state == STATE_RAW);
        if (strcmp(cs, "utf-8") == 0) {
            tok->encoding = cs;
        } else {
            r = set_readline(tok, cs);
            if (r) {
                tok->encoding = cs;
                tok->decoding_state = STATE_NORMAL;
            }
            else {
                PyErr_Format(PyExc_SyntaxError,
                             "encoding problem: %s", cs);
                PyMem_FREE(cs);
            }
        }
    } else {                /* then, compare cs with BOM */
        r = (strcmp(tok->encoding, cs) == 0);
        if (!r)
            PyErr_Format(PyExc_SyntaxError,
                         "encoding problem: %s with BOM", cs);
        PyMem_FREE(cs);
    }
    return r;
}