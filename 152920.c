decode_utf8(struct compiling *c, const char **sPtr, const char *end)
{
#ifndef Py_USING_UNICODE
        Py_FatalError("decode_utf8 should not be called in this build.");
        return NULL;
#else
    const char *s, *t;
    t = s = *sPtr;
    /* while (s < end && *s != '\\') s++; */ /* inefficient for u".." */
    while (s < end && (*s & 0x80)) s++;
    *sPtr = s;
    return PyUnicode_DecodeUTF8(t, s - t, NULL);
#endif
}