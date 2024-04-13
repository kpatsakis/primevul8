PyTokenizer_FromString(const char *str, int exec_input)
{
    struct tok_state *tok = tok_new();
    if (tok == NULL)
        return NULL;
    str = decode_str(str, exec_input, tok);
    if (str == NULL) {
        PyTokenizer_Free(tok);
        return NULL;
    }

    /* XXX: constify members. */
    tok->buf = tok->cur = tok->end = tok->inp = (char*)str;
    return tok;
}