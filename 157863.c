builtin_any(PyObject *module, PyObject *iterable)
/*[clinic end generated code: output=fa65684748caa60e input=41d7451c23384f24]*/
{
    PyObject *it, *item;
    PyObject *(*iternext)(PyObject *);
    int cmp;

    it = PyObject_GetIter(iterable);
    if (it == NULL)
        return NULL;
    iternext = *Py_TYPE(it)->tp_iternext;

    for (;;) {
        item = iternext(it);
        if (item == NULL)
            break;
        cmp = PyObject_IsTrue(item);
        Py_DECREF(item);
        if (cmp < 0) {
            Py_DECREF(it);
            return NULL;
        }
        if (cmp > 0) {
            Py_DECREF(it);
            Py_RETURN_TRUE;
        }
    }
    Py_DECREF(it);
    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration))
            PyErr_Clear();
        else
            return NULL;
    }
    Py_RETURN_FALSE;
}