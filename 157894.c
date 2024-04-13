builtin_dir(PyObject *self, PyObject *args)
{
    PyObject *arg = NULL;

    if (!PyArg_UnpackTuple(args, "dir", 0, 1, &arg))
        return NULL;
    return PyObject_Dir(arg);
}