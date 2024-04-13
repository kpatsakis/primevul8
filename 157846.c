builtin_getattr(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
    PyObject *v, *name, *result;

    if (!_PyArg_CheckPositional("getattr", nargs, 2, 3))
        return NULL;

    v = args[0];
    name = args[1];
    if (!PyUnicode_Check(name)) {
        PyErr_SetString(PyExc_TypeError,
                        "getattr(): attribute name must be string");
        return NULL;
    }
    if (nargs > 2) {
        if (_PyObject_LookupAttr(v, name, &result) == 0) {
            PyObject *dflt = args[2];
            Py_INCREF(dflt);
            return dflt;
        }
    }
    else {
        result = PyObject_GetAttr(v, name);
    }
    return result;
}