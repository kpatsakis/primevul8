decode_str(const char *input, int single, struct tok_state *tok)
{
    PyObject* utf8 = NULL;
    const char *str;
    const char *s;
    const char *newl[2] = {NULL, NULL};
    int lineno = 0;
    tok->input = str = translate_newlines(input, single, tok);
    if (str == NULL)
        return NULL;
    tok->enc = NULL;
    tok->str = str;
    if (!check_bom(buf_getc, buf_ungetc, buf_setreadl, tok))
        return error_ret(tok);
    str = tok->str;             /* string after BOM if any */
    assert(str);
    if (tok->enc != NULL) {
        utf8 = translate_into_utf8(str, tok->enc);
        if (utf8 == NULL)
            return error_ret(tok);
        str = PyBytes_AsString(utf8);
    }
    for (s = str;; s++) {
        if (*s == '\0') break;
        else if (*s == '\n') {
            assert(lineno < 2);
            newl[lineno] = s;
            lineno++;
            if (lineno == 2) break;
        }
    }
    tok->enc = NULL;
    /* need to check line 1 and 2 separately since check_coding_spec
       assumes a single line as input */
    if (newl[0]) {
        if (!check_coding_spec(str, newl[0] - str, tok, buf_setreadl))
            return error_ret(tok);
        if (tok->enc == NULL && !tok->read_coding_spec && newl[1]) {
            if (!check_coding_spec(newl[0]+1, newl[1] - newl[0],
                                   tok, buf_setreadl))
                return error_ret(tok);
        }
    }
    if (tok->enc != NULL) {
        assert(utf8 == NULL);
        utf8 = translate_into_utf8(str, tok->enc);
        if (utf8 == NULL)
            return error_ret(tok);
        str = PyBytes_AS_STRING(utf8);
    }
    assert(tok->decoding_buffer == NULL);
    tok->decoding_buffer = utf8; /* CAUTION */
    return str;
}