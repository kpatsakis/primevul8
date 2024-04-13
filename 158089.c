builtin_iter(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
    PyObject *v;

    if (!_PyArg_CheckPositional("iter", nargs, 1, 2))
        return NULL;
    v = args[0];
    if (nargs == 1)
        return PyObject_GetIter(v);
    if (!PyCallable_Check(v)) {
        PyErr_SetString(PyExc_TypeError,
                        "iter(v, w): v must be callable");
        return NULL;
    }
    PyObject *sentinel = args[1];
    return PyCallIter_New(v, sentinel);
}