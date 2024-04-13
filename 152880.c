tok_nextc(struct tok_state *tok)
{
    for (;;) {
        if (tok->cur != tok->inp) {
            return Py_CHARMASK(*tok->cur++); /* Fast path */
        }
        if (tok->done != E_OK)
            return EOF;
        if (tok->fp == NULL) {
            char *end = strchr(tok->inp, '\n');
            if (end != NULL)
                end++;
            else {
                end = strchr(tok->inp, '\0');
                if (end == tok->inp) {
                    tok->done = E_EOF;
                    return EOF;
                }
            }
            if (tok->start == NULL)
                tok->buf = tok->cur;
            tok->line_start = tok->cur;
            tok->lineno++;
            tok->inp = end;
            return Py_CHARMASK(*tok->cur++);
        }
        if (tok->prompt != NULL) {
            char *newtok = PyOS_Readline(stdin, stdout, tok->prompt);
#ifndef PGEN
            if (newtok != NULL) {
                char *translated = translate_newlines(newtok, 0, tok);
                PyMem_FREE(newtok);
                if (translated == NULL)
                    return EOF;
                newtok = translated;
            }
            if (tok->encoding && newtok && *newtok) {
                /* Recode to UTF-8 */
                Py_ssize_t buflen;
                const char* buf;
                PyObject *u = translate_into_utf8(newtok, tok->encoding);
                PyMem_FREE(newtok);
                if (!u) {
                    tok->done = E_DECODE;
                    return EOF;
                }
                buflen = PyBytes_GET_SIZE(u);
                buf = PyBytes_AS_STRING(u);
                newtok = PyMem_MALLOC(buflen+1);
                if (newtok == NULL) {
                    Py_DECREF(u);
                    tok->done = E_NOMEM;
                    return EOF;
                }
                strcpy(newtok, buf);
                Py_DECREF(u);
            }
#endif
            if (tok->nextprompt != NULL)
                tok->prompt = tok->nextprompt;
            if (newtok == NULL)
                tok->done = E_INTR;
            else if (*newtok == '\0') {
                PyMem_FREE(newtok);
                tok->done = E_EOF;
            }
            else if (tok->start != NULL) {
                size_t start = tok->start - tok->buf;
                size_t oldlen = tok->cur - tok->buf;
                size_t newlen = oldlen + strlen(newtok);
                char *buf = tok->buf;
                buf = (char *)PyMem_REALLOC(buf, newlen+1);
                tok->lineno++;
                if (buf == NULL) {
                    PyMem_FREE(tok->buf);
                    tok->buf = NULL;
                    PyMem_FREE(newtok);
                    tok->done = E_NOMEM;
                    return EOF;
                }
                tok->buf = buf;
                tok->cur = tok->buf + oldlen;
                tok->line_start = tok->cur;
                strcpy(tok->buf + oldlen, newtok);
                PyMem_FREE(newtok);
                tok->inp = tok->buf + newlen;
                tok->end = tok->inp + 1;
                tok->start = tok->buf + start;
            }
            else {
                tok->lineno++;
                if (tok->buf != NULL)
                    PyMem_FREE(tok->buf);
                tok->buf = newtok;
                tok->cur = tok->buf;
                tok->line_start = tok->buf;
                tok->inp = strchr(tok->buf, '\0');
                tok->end = tok->inp + 1;
            }
        }
        else {
            int done = 0;
            Py_ssize_t cur = 0;
            char *pt;
            if (tok->start == NULL) {
                if (tok->buf == NULL) {
                    tok->buf = (char *)
                        PyMem_MALLOC(BUFSIZ);
                    if (tok->buf == NULL) {
                        tok->done = E_NOMEM;
                        return EOF;
                    }
                    tok->end = tok->buf + BUFSIZ;
                }
                if (decoding_fgets(tok->buf, (int)(tok->end - tok->buf),
                          tok) == NULL) {
                    if (!tok->decoding_erred)
                        tok->done = E_EOF;
                    done = 1;
                }
                else {
                    tok->done = E_OK;
                    tok->inp = strchr(tok->buf, '\0');
                    done = tok->inp == tok->buf || tok->inp[-1] == '\n';
                }
            }
            else {
                cur = tok->cur - tok->buf;
                if (decoding_feof(tok)) {
                    tok->done = E_EOF;
                    done = 1;
                }
                else
                    tok->done = E_OK;
            }
            tok->lineno++;
            /* Read until '\n' or EOF */
            while (!done) {
                Py_ssize_t curstart = tok->start == NULL ? -1 :
                          tok->start - tok->buf;
                Py_ssize_t curvalid = tok->inp - tok->buf;
                Py_ssize_t newsize = curvalid + BUFSIZ;
                char *newbuf = tok->buf;
                newbuf = (char *)PyMem_REALLOC(newbuf,
                                               newsize);
                if (newbuf == NULL) {
                    tok->done = E_NOMEM;
                    tok->cur = tok->inp;
                    return EOF;
                }
                tok->buf = newbuf;
                tok->cur = tok->buf + cur;
                tok->line_start = tok->cur;
                tok->inp = tok->buf + curvalid;
                tok->end = tok->buf + newsize;
                tok->start = curstart < 0 ? NULL :
                         tok->buf + curstart;
                if (decoding_fgets(tok->inp,
                               (int)(tok->end - tok->inp),
                               tok) == NULL) {
                    /* Break out early on decoding
                       errors, as tok->buf will be NULL
                     */
                    if (tok->decoding_erred)
                        return EOF;
                    /* Last line does not end in \n,
                       fake one */
                    strcpy(tok->inp, "\n");
                }
                tok->inp = strchr(tok->inp, '\0');
                done = tok->inp[-1] == '\n';
            }
            if (tok->buf != NULL) {
                tok->cur = tok->buf + cur;
                tok->line_start = tok->cur;
                /* replace "\r\n" with "\n" */
                /* For Mac leave the \r, giving a syntax error */
                pt = tok->inp - 2;
                if (pt >= tok->buf && *pt == '\r') {
                    *pt++ = '\n';
                    *pt = '\0';
                    tok->inp = pt;
                }
            }
        }
        if (tok->done != E_OK) {
            if (tok->prompt != NULL)
                PySys_WriteStderr("\n");
            tok->cur = tok->inp;
            return EOF;
        }
    }
    /*NOTREACHED*/
}