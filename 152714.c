tok_backup(struct tok_state *tok, int c)
{
    if (c != EOF) {
        if (--tok->cur < tok->buf)
            Py_FatalError("tok_backup: beginning of buffer");
        if (*tok->cur != c)
            *tok->cur = c;
    }
}