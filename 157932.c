builtin_breakpoint(PyObject *self, PyObject *const *args, Py_ssize_t nargs, PyObject *keywords)
{
    PyObject *hook = PySys_GetObject("breakpointhook");

    if (hook == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "lost sys.breakpointhook");
        return NULL;
    }
    Py_INCREF(hook);
    PyObject *retval = _PyObject_FastCallKeywords(hook, args, nargs, keywords);
    Py_DECREF(hook);
    return retval;
}