decoding_fgets(char *s, int size, struct tok_state *tok)
{
    char *line = NULL;
    int badchar = 0;
    for (;;) {
        if (tok->decoding_state == STATE_NORMAL) {
            /* We already have a codec associated with
               this input. */
            line = fp_readl(s, size, tok);
            break;
        } else if (tok->decoding_state == STATE_RAW) {
            /* We want a 'raw' read. */
            line = Py_UniversalNewlineFgets(s, size,
                                            tok->fp, NULL);
            break;
        } else {
            /* We have not yet determined the encoding.
               If an encoding is found, use the file-pointer
               reader functions from now on. */
            if (!check_bom(fp_getc, fp_ungetc, fp_setreadl, tok))
                return error_ret(tok);
            assert(tok->decoding_state != STATE_INIT);
        }
    }
    if (line != NULL && tok->lineno < 2 && !tok->read_coding_spec) {
        if (!check_coding_spec(line, strlen(line), tok, fp_setreadl)) {
            return error_ret(tok);
        }
    }
#ifndef PGEN
    /* The default encoding is UTF-8, so make sure we don't have any
       non-UTF-8 sequences in it. */
    if (line && !tok->encoding) {
        unsigned char *c;
        int length;
        for (c = (unsigned char *)line; *c; c += length)
            if (!(length = valid_utf8(c))) {
                badchar = *c;
                break;
            }
    }
    if (badchar) {
        /* Need to add 1 to the line number, since this line
           has not been counted, yet.  */
        PyErr_Format(PyExc_SyntaxError,
                "Non-UTF-8 code starting with '\\x%.2x' "
                "in file %U on line %i, "
                "but no encoding declared; "
                "see http://python.org/dev/peps/pep-0263/ for details",
                badchar, tok->filename, tok->lineno + 1);
        return error_ret(tok);
    }
#endif
    return line;
}