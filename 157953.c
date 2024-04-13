builtin_max(PyObject *self, PyObject *args, PyObject *kwds)
{
    return min_max(args, kwds, Py_GT);
}