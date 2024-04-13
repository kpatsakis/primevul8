ast_error(struct compiling *c, const node *n, const char *errmsg, ...)
{
    PyObject *value, *errstr, *loc, *tmp;
    va_list va;

    va_start(va, errmsg);
    errstr = PyUnicode_FromFormatV(errmsg, va);
    va_end(va);
    if (!errstr) {
        return 0;
    }
    loc = PyErr_ProgramTextObject(c->c_filename, LINENO(n));
    if (!loc) {
        Py_INCREF(Py_None);
        loc = Py_None;
    }
    tmp = Py_BuildValue("(OiiN)", c->c_filename, LINENO(n), n->n_col_offset + 1, loc);
    if (!tmp) {
        Py_DECREF(errstr);
        return 0;
    }
    value = PyTuple_Pack(2, errstr, tmp);
    Py_DECREF(errstr);
    Py_DECREF(tmp);
    if (value) {
        PyErr_SetObject(PyExc_SyntaxError, value);
        Py_DECREF(value);
    }
    return 0;
}