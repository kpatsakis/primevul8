PyTokenizer_FromFile(FILE *fp, const char* enc,
                     const char *ps1, const char *ps2)
{
    struct tok_state *tok = tok_new();
    if (tok == NULL)
        return NULL;
    if ((tok->buf = (char *)PyMem_MALLOC(BUFSIZ)) == NULL) {
        PyTokenizer_Free(tok);
        return NULL;
    }
    tok->cur = tok->inp = tok->buf;
    tok->end = tok->buf + BUFSIZ;
    tok->fp = fp;
    tok->prompt = ps1;
    tok->nextprompt = ps2;
    if (enc != NULL) {
        /* Must copy encoding declaration since it
           gets copied into the parse tree. */
        tok->encoding = PyMem_MALLOC(strlen(enc)+1);
        if (!tok->encoding) {
            PyTokenizer_Free(tok);
            return NULL;
        }
        strcpy(tok->encoding, enc);
        tok->decoding_state = STATE_NORMAL;
    }
    return tok;
}