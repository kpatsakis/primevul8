filter_next(filterobject *lz)
{
    PyObject *item;
    PyObject *it = lz->it;
    long ok;
    PyObject *(*iternext)(PyObject *);
    int checktrue = lz->func == Py_None || lz->func == (PyObject *)&PyBool_Type;

    iternext = *Py_TYPE(it)->tp_iternext;
    for (;;) {
        item = iternext(it);
        if (item == NULL)
            return NULL;

        if (checktrue) {
            ok = PyObject_IsTrue(item);
        } else {
            PyObject *good;
            good = PyObject_CallFunctionObjArgs(lz->func, item, NULL);
            if (good == NULL) {
                Py_DECREF(item);
                return NULL;
            }
            ok = PyObject_IsTrue(good);
            Py_DECREF(good);
        }
        if (ok > 0)
            return item;
        Py_DECREF(item);
        if (ok < 0)
            return NULL;
    }
}