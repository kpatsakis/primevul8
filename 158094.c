builtin_min(PyObject *self, PyObject *args, PyObject *kwds)
{
    return min_max(args, kwds, Py_LT);
}