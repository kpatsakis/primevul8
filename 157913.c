flush_io(void)
{
    PyObject *f, *r;
    PyObject *type, *value, *traceback;

    /* Save the current exception */
    PyErr_Fetch(&type, &value, &traceback);

    f = _PySys_GetObjectId(&PyId_stderr);
    if (f != NULL) {
        r = _PyObject_CallMethodId(f, &PyId_flush, NULL);
        if (r)
            Py_DECREF(r);
        else
            PyErr_Clear();
    }
    f = _PySys_GetObjectId(&PyId_stdout);
    if (f != NULL) {
        r = _PyObject_CallMethodId(f, &PyId_flush, NULL);
        if (r)
            Py_DECREF(r);
        else
            PyErr_Clear();
    }

    PyErr_Restore(type, value, traceback);
}