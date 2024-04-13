map_reduce(mapobject *lz, PyObject *Py_UNUSED(ignored))
{
    Py_ssize_t numargs = PyTuple_GET_SIZE(lz->iters);
    PyObject *args = PyTuple_New(numargs+1);
    Py_ssize_t i;
    if (args == NULL)
        return NULL;
    Py_INCREF(lz->func);
    PyTuple_SET_ITEM(args, 0, lz->func);
    for (i = 0; i<numargs; i++){
        PyObject *it = PyTuple_GET_ITEM(lz->iters, i);
        Py_INCREF(it);
        PyTuple_SET_ITEM(args, i+1, it);
    }

    return Py_BuildValue("ON", Py_TYPE(lz), args);
}