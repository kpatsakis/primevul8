warn_invalid_escape_sequence(struct compiling *c, const node *n,
                             unsigned char first_invalid_escape_char)
{
    PyObject *msg = PyUnicode_FromFormat("invalid escape sequence \\%c",
                                         first_invalid_escape_char);
    if (msg == NULL) {
        return -1;
    }
    if (PyErr_WarnExplicitObject(PyExc_SyntaxWarning, msg,
                                   c->c_filename, LINENO(n),
                                   NULL, NULL) < 0)
    {
        if (PyErr_ExceptionMatches(PyExc_SyntaxWarning)) {
            /* Replace the SyntaxWarning exception with a SyntaxError
               to get a more accurate error report */
            PyErr_Clear();
            ast_error(c, n, "%U", msg);
        }
        Py_DECREF(msg);
        return -1;
    }
    Py_DECREF(msg);
    return 0;
}