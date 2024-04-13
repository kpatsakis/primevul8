zip_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    zipobject *lz;
    Py_ssize_t i;
    PyObject *ittuple;  /* tuple of iterators */
    PyObject *result;
    Py_ssize_t tuplesize;

    if (type == &PyZip_Type && !_PyArg_NoKeywords("zip", kwds))
        return NULL;

    /* args must be a tuple */
    assert(PyTuple_Check(args));
    tuplesize = PyTuple_GET_SIZE(args);

    /* obtain iterators */
    ittuple = PyTuple_New(tuplesize);
    if (ittuple == NULL)
        return NULL;
    for (i=0; i < tuplesize; ++i) {
        PyObject *item = PyTuple_GET_ITEM(args, i);
        PyObject *it = PyObject_GetIter(item);
        if (it == NULL) {
            if (PyErr_ExceptionMatches(PyExc_TypeError))
                PyErr_Format(PyExc_TypeError,
                    "zip argument #%zd must support iteration",
                    i+1);
            Py_DECREF(ittuple);
            return NULL;
        }
        PyTuple_SET_ITEM(ittuple, i, it);
    }

    /* create a result holder */
    result = PyTuple_New(tuplesize);
    if (result == NULL) {
        Py_DECREF(ittuple);
        return NULL;
    }
    for (i=0 ; i < tuplesize ; i++) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(result, i, Py_None);
    }

    /* create zipobject structure */
    lz = (zipobject *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(ittuple);
        Py_DECREF(result);
        return NULL;
    }
    lz->ittuple = ittuple;
    lz->tuplesize = tuplesize;
    lz->result = result;

    return (PyObject *)lz;
}