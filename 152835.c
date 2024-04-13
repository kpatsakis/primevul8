parsestr(struct compiling *c, const node *n, const char *s)
{
        size_t len, i;
        int quote = Py_CHARMASK(*s);
        int rawmode = 0;
        int need_encoding;
        int unicode = c->c_future_unicode;
        int bytes = 0;

        if (isalpha(quote) || quote == '_') {
                if (quote == 'u' || quote == 'U') {
                        quote = *++s;
                        unicode = 1;
                }
                if (quote == 'b' || quote == 'B') {
                        quote = *++s;
                        unicode = 0;
                        bytes = 1;
                }
                if (quote == 'r' || quote == 'R') {
                        quote = *++s;
                        rawmode = 1;
                }
        }
        if (quote != '\'' && quote != '\"') {
                PyErr_BadInternalCall();
                return NULL;
        }
        s++;
        len = strlen(s);
        if (len > INT_MAX) {
                PyErr_SetString(PyExc_OverflowError,
                                "string to parse is too long");
                return NULL;
        }
        if (s[--len] != quote) {
                PyErr_BadInternalCall();
                return NULL;
        }
        if (len >= 4 && s[0] == quote && s[1] == quote) {
                s += 2;
                len -= 2;
                if (s[--len] != quote || s[--len] != quote) {
                        PyErr_BadInternalCall();
                        return NULL;
                }
        }
        if (Py_Py3kWarningFlag && bytes) {
            for (i = 0; i < len; i++) {
                if ((unsigned char)s[i] > 127) {
                    if (!ast_warn(c, n,
                        "non-ascii bytes literals not supported in 3.x"))
                        return NULL;
                    break;
                }
            }
        }
#ifdef Py_USING_UNICODE
        if (unicode || Py_UnicodeFlag) {
                return decode_unicode(c, s, len, rawmode, c->c_encoding);
        }
#endif
        need_encoding = (c->c_encoding != NULL &&
                         strcmp(c->c_encoding, "utf-8") != 0 &&
                         strcmp(c->c_encoding, "iso-8859-1") != 0);
        if (rawmode || strchr(s, '\\') == NULL) {
                if (need_encoding) {
#ifndef Py_USING_UNICODE
                        /* This should not happen - we never see any other
                           encoding. */
                        Py_FatalError(
                            "cannot deal with encodings in this build.");
#else
                        PyObject *v, *u = PyUnicode_DecodeUTF8(s, len, NULL);
                        if (u == NULL)
                                return NULL;
                        v = PyUnicode_AsEncodedString(u, c->c_encoding, NULL);
                        Py_DECREF(u);
                        return v;
#endif
                } else {
                  return PyBytes_FromStringAndSize(s, len);
                }
        }

        return PyBytes_DecodeEscape(s, len, NULL, 1,
                                    need_encoding ? c->c_encoding : NULL);
}