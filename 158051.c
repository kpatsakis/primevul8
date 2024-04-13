PyErr_Display(PyObject *exception, PyObject *value, PyObject *tb)
{
    PyObject *seen;
    PyObject *f = _PySys_GetObjectId(&PyId_stderr);
    if (PyExceptionInstance_Check(value)
        && tb != NULL && PyTraceBack_Check(tb)) {
        /* Put the traceback on the exception, otherwise it won't get
           displayed.  See issue #18776. */
        PyObject *cur_tb = PyException_GetTraceback(value);
        if (cur_tb == NULL)
            PyException_SetTraceback(value, tb);
        else
            Py_DECREF(cur_tb);
    }
    if (f == Py_None) {
        /* pass */
    }
    else if (f == NULL) {
        _PyObject_Dump(value);
        fprintf(stderr, "lost sys.stderr\n");
    }
    else {
        /* We choose to ignore seen being possibly NULL, and report
           at least the main exception (it could be a MemoryError).
        */
        seen = PySet_New(NULL);
        if (seen == NULL)
            PyErr_Clear();
        print_exception_recursive(f, value, seen);
        Py_XDECREF(seen);
    }
}