verify_identifier(struct tok_state *tok)
{
    PyObject *s;
    int result;
    if (tok->decoding_erred)
        return 0;
    s = PyUnicode_DecodeUTF8(tok->start, tok->cur - tok->start, NULL);
    if (s == NULL) {
        if (PyErr_ExceptionMatches(PyExc_UnicodeDecodeError)) {
            PyErr_Clear();
            tok->done = E_IDENTIFIER;
        } else {
            tok->done = E_ERROR;
        }
        return 0;
    }
    result = PyUnicode_IsIdentifier(s);
    Py_DECREF(s);
    if (result == 0)
        tok->done = E_IDENTIFIER;
    return result;
}