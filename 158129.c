filter_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyObject *func, *seq;
    PyObject *it;
    filterobject *lz;

    if (type == &PyFilter_Type && !_PyArg_NoKeywords("filter", kwds))
        return NULL;

    if (!PyArg_UnpackTuple(args, "filter", 2, 2, &func, &seq))
        return NULL;

    /* Get iterator. */
    it = PyObject_GetIter(seq);
    if (it == NULL)
        return NULL;

    /* create filterobject structure */
    lz = (filterobject *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(it);
        return NULL;
    }
    Py_INCREF(func);
    lz->func = func;
    lz->it = it;

    return (PyObject *)lz;
}