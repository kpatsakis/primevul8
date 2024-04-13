syntaxerror(struct tok_state *tok, const char *format, ...)
{
#ifndef PGEN
    va_list vargs;
#ifdef HAVE_STDARG_PROTOTYPES
    va_start(vargs, format);
#else
    va_start(vargs);
#endif
    PyErr_FormatV(PyExc_SyntaxError, format, vargs);
    va_end(vargs);
    PyErr_SyntaxLocationObject(tok->filename,
                               tok->lineno,
                               (int)(tok->cur - tok->line_start));
    tok->done = E_ERROR;
#else
    tok->done = E_TOKEN;
#endif
    return ERRORTOKEN;
}