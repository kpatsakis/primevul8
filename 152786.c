Ta3Tokenizer_Free(struct tok_state *tok)
{
    if (tok->encoding != NULL)
        PyMem_FREE(tok->encoding);
#ifndef PGEN
    Py_XDECREF(tok->decoding_readline);
    Py_XDECREF(tok->decoding_buffer);
    Py_XDECREF(tok->filename);
#endif
    if (tok->fp != NULL && tok->buf != NULL)
        PyMem_FREE(tok->buf);
    if (tok->input)
        PyMem_FREE((char *)tok->input);
    PyMem_FREE(tok);
}