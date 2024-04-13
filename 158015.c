decode_unicode_with_escapes(struct compiling *c, const node *n, const char *s,
                            size_t len)
{
    PyObject *v, *u;
    char *buf;
    char *p;
    const char *end;

    /* check for integer overflow */
    if (len > SIZE_MAX / 6)
        return NULL;
    /* "ä" (2 bytes) may become "\U000000E4" (10 bytes), or 1:5
       "\ä" (3 bytes) may become "\u005c\U000000E4" (16 bytes), or ~1:6 */
    u = PyBytes_FromStringAndSize((char *)NULL, len * 6);
    if (u == NULL)
        return NULL;
    p = buf = PyBytes_AsString(u);
    end = s + len;
    while (s < end) {
        if (*s == '\\') {
            *p++ = *s++;
            if (s >= end || *s & 0x80) {
                strcpy(p, "u005c");
                p += 5;
                if (s >= end)
                    break;
            }
        }
        if (*s & 0x80) { /* XXX inefficient */
            PyObject *w;
            int kind;
            void *data;
            Py_ssize_t len, i;
            w = decode_utf8(c, &s, end);
            if (w == NULL) {
                Py_DECREF(u);
                return NULL;
            }
            kind = PyUnicode_KIND(w);
            data = PyUnicode_DATA(w);
            len = PyUnicode_GET_LENGTH(w);
            for (i = 0; i < len; i++) {
                Py_UCS4 chr = PyUnicode_READ(kind, data, i);
                sprintf(p, "\\U%08x", chr);
                p += 10;
            }
            /* Should be impossible to overflow */
            assert(p - buf <= PyBytes_GET_SIZE(u));
            Py_DECREF(w);
        } else {
            *p++ = *s++;
        }
    }
    len = p - buf;
    s = buf;

    const char *first_invalid_escape;
    v = _PyUnicode_DecodeUnicodeEscape(s, len, NULL, &first_invalid_escape);

    if (v != NULL && first_invalid_escape != NULL) {
        if (warn_invalid_escape_sequence(c, n, *first_invalid_escape) < 0) {
            /* We have not decref u before because first_invalid_escape points
               inside u. */
            Py_XDECREF(u);
            Py_DECREF(v);
            return NULL;
        }
    }
    Py_XDECREF(u);
    return v;
}