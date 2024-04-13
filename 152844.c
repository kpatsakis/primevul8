error_ret(struct tok_state *tok) /* XXX */
{
    tok->decoding_erred = 1;
    if (tok->fp != NULL && tok->buf != NULL) /* see Ta3Tokenizer_Free */
        PyMem_FREE(tok->buf);
    tok->buf = tok->cur = tok->end = tok->inp = tok->start = NULL;
    tok->done = E_DECODE;
    return NULL;                /* as if it were EOF */
}