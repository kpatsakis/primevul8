builtin_sorted(PyObject *self, PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames)
{
    PyObject *newlist, *v, *seq, *callable;

    /* Keyword arguments are passed through list.sort() which will check
       them. */
    if (!_PyArg_UnpackStack(args, nargs, "sorted", 1, 1, &seq))
        return NULL;

    newlist = PySequence_List(seq);
    if (newlist == NULL)
        return NULL;

    callable = _PyObject_GetAttrId(newlist, &PyId_sort);
    if (callable == NULL) {
        Py_DECREF(newlist);
        return NULL;
    }

    assert(nargs >= 1);
    v = _PyObject_FastCallKeywords(callable, args + 1, nargs - 1, kwnames);
    Py_DECREF(callable);
    if (v == NULL) {
        Py_DECREF(newlist);
        return NULL;
    }
    Py_DECREF(v);
    return newlist;
}