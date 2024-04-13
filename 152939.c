ast_type_init(PyObject *self, PyObject *args, PyObject *kw)
{
    Py_ssize_t i, numfields = 0;
    int res = -1;
    PyObject *key, *value, *fields;
    if (lookup_attr_id((PyObject*)Py_TYPE(self), &PyId__fields, &fields) < 0) {
        goto cleanup;
    }
    if (fields) {
        numfields = PySequence_Size(fields);
        if (numfields == -1)
            goto cleanup;
    }

    res = 0; /* if no error occurs, this stays 0 to the end */
    if (numfields < PyTuple_GET_SIZE(args)) {
        PyErr_Format(PyExc_TypeError, "%.400s constructor takes at most "
                     "%zd positional argument%s",
                     Py_TYPE(self)->tp_name,
                     numfields, numfields == 1 ? "" : "s");
        res = -1;
        goto cleanup;
    }
    for (i = 0; i < PyTuple_GET_SIZE(args); i++) {
        /* cannot be reached when fields is NULL */
        PyObject *name = PySequence_GetItem(fields, i);
        if (!name) {
            res = -1;
            goto cleanup;
        }
        res = PyObject_SetAttr(self, name, PyTuple_GET_ITEM(args, i));
        Py_DECREF(name);
        if (res < 0)
            goto cleanup;
    }
    if (kw) {
        i = 0;  /* needed by PyDict_Next */
        while (PyDict_Next(kw, &i, &key, &value)) {
            res = PyObject_SetAttr(self, key, value);
            if (res < 0)
                goto cleanup;
        }
    }
  cleanup:
    Py_XDECREF(fields);
    return res;
}