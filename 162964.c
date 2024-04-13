static PyObject *Input_close(InputObject *self, PyObject *args)
{
    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}