ast_error_finish(const char *filename)
{
    PyObject *type, *value, *tback, *errstr, *loc, *tmp;
    long lineno;

    assert(PyErr_Occurred());
    if (!PyErr_ExceptionMatches(PyExc_SyntaxError))
        return;

    PyErr_Fetch(&type, &value, &tback);
    errstr = PyTuple_GetItem(value, 0);
    if (!errstr)
        return;
    Py_INCREF(errstr);
    lineno = PyLong_AsLong(PyTuple_GetItem(value, 1));
    if (lineno == -1) {
        Py_DECREF(errstr);
        return;
    }
    Py_DECREF(value);

    loc = PyErr_ProgramText(filename, lineno);
    if (!loc) {
        Py_INCREF(Py_None);
        loc = Py_None;
    }
    tmp = Py_BuildValue("(zlOO)", filename, lineno, Py_None, loc);
    Py_DECREF(loc);
    if (!tmp) {
        Py_DECREF(errstr);
        return;
    }
    value = PyTuple_Pack(2, errstr, tmp);
    Py_DECREF(errstr);
    Py_DECREF(tmp);
    if (!value)
        return;
    PyErr_Restore(type, value, tback);
}