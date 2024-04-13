zip_next(zipobject *lz)
{
    Py_ssize_t i;
    Py_ssize_t tuplesize = lz->tuplesize;
    PyObject *result = lz->result;
    PyObject *it;
    PyObject *item;
    PyObject *olditem;

    if (tuplesize == 0)
        return NULL;
    if (Py_REFCNT(result) == 1) {
        Py_INCREF(result);
        for (i=0 ; i < tuplesize ; i++) {
            it = PyTuple_GET_ITEM(lz->ittuple, i);
            item = (*Py_TYPE(it)->tp_iternext)(it);
            if (item == NULL) {
                Py_DECREF(result);
                return NULL;
            }
            olditem = PyTuple_GET_ITEM(result, i);
            PyTuple_SET_ITEM(result, i, item);
            Py_DECREF(olditem);
        }
    } else {
        result = PyTuple_New(tuplesize);
        if (result == NULL)
            return NULL;
        for (i=0 ; i < tuplesize ; i++) {
            it = PyTuple_GET_ITEM(lz->ittuple, i);
            item = (*Py_TYPE(it)->tp_iternext)(it);
            if (item == NULL) {
                Py_DECREF(result);
                return NULL;
            }
            PyTuple_SET_ITEM(result, i, item);
        }
    }
    return result;
}