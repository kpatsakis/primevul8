PyTokenizer_FromUTF8(const char *str, int exec_input)
{
    struct tok_state *tok = tok_new();
    if (tok == NULL)
        return NULL;
#ifndef PGEN
    tok->input = str = translate_newlines(str, exec_input, tok);
#endif
    if (str == NULL) {
        PyTokenizer_Free(tok);
        return NULL;
    }
    tok->decoding_state = STATE_RAW;
    tok->read_coding_spec = 1;
    tok->enc = NULL;
    tok->str = str;
    tok->encoding = (char *)PyMem_MALLOC(6);
    if (!tok->encoding) {
        PyTokenizer_Free(tok);
        return NULL;
    }
    strcpy(tok->encoding, "utf-8");

    /* XXX: constify members. */
    tok->buf = tok->cur = tok->end = tok->inp = (char*)str;
    return tok;
}