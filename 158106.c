map_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyObject *it, *iters, *func;
    mapobject *lz;
    Py_ssize_t numargs, i;

    if (type == &PyMap_Type && !_PyArg_NoKeywords("map", kwds))
        return NULL;

    numargs = PyTuple_Size(args);
    if (numargs < 2) {
        PyErr_SetString(PyExc_TypeError,
           "map() must have at least two arguments.");
        return NULL;
    }

    iters = PyTuple_New(numargs-1);
    if (iters == NULL)
        return NULL;

    for (i=1 ; i<numargs ; i++) {
        /* Get iterator. */
        it = PyObject_GetIter(PyTuple_GET_ITEM(args, i));
        if (it == NULL) {
            Py_DECREF(iters);
            return NULL;
        }
        PyTuple_SET_ITEM(iters, i-1, it);
    }

    /* create mapobject structure */
    lz = (mapobject *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(iters);
        return NULL;
    }
    lz->iters = iters;
    func = PyTuple_GET_ITEM(args, 0);
    Py_INCREF(func);
    lz->func = func;

    return (PyObject *)lz;
}