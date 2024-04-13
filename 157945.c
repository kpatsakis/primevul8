builtin_vars(PyObject *self, PyObject *args)
{
    PyObject *v = NULL;
    PyObject *d;

    if (!PyArg_UnpackTuple(args, "vars", 0, 1, &v))
        return NULL;
    if (v == NULL) {
        d = PyEval_GetLocals();
        if (d == NULL)
            return NULL;
        Py_INCREF(d);
    }
    else {
        d = _PyObject_GetAttrId(v, &PyId___dict__);
        if (d == NULL) {
            PyErr_SetString(PyExc_TypeError,
                "vars() argument must have __dict__ attribute");
            return NULL;
        }
    }
    return d;
}