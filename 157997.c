min_max(PyObject *args, PyObject *kwds, int op)
{
    PyObject *v, *it, *item, *val, *maxitem, *maxval, *keyfunc=NULL;
    PyObject *emptytuple, *defaultval = NULL;
    static char *kwlist[] = {"key", "default", NULL};
    const char *name = op == Py_LT ? "min" : "max";
    const int positional = PyTuple_Size(args) > 1;
    int ret;

    if (positional)
        v = args;
    else if (!PyArg_UnpackTuple(args, name, 1, 1, &v))
        return NULL;

    emptytuple = PyTuple_New(0);
    if (emptytuple == NULL)
        return NULL;
    ret = PyArg_ParseTupleAndKeywords(emptytuple, kwds,
                                      (op == Py_LT) ? "|$OO:min" : "|$OO:max",
                                      kwlist, &keyfunc, &defaultval);
    Py_DECREF(emptytuple);
    if (!ret)
        return NULL;

    if (positional && defaultval != NULL) {
        PyErr_Format(PyExc_TypeError,
                        "Cannot specify a default for %s() with multiple "
                        "positional arguments", name);
        return NULL;
    }

    it = PyObject_GetIter(v);
    if (it == NULL) {
        return NULL;
    }

    if (keyfunc == Py_None) {
        keyfunc = NULL;
    }

    maxitem = NULL; /* the result */
    maxval = NULL;  /* the value associated with the result */
    while (( item = PyIter_Next(it) )) {
        /* get the value from the key function */
        if (keyfunc != NULL) {
            val = PyObject_CallFunctionObjArgs(keyfunc, item, NULL);
            if (val == NULL)
                goto Fail_it_item;
        }
        /* no key function; the value is the item */
        else {
            val = item;
            Py_INCREF(val);
        }

        /* maximum value and item are unset; set them */
        if (maxval == NULL) {
            maxitem = item;
            maxval = val;
        }
        /* maximum value and item are set; update them as necessary */
        else {
            int cmp = PyObject_RichCompareBool(val, maxval, op);
            if (cmp < 0)
                goto Fail_it_item_and_val;
            else if (cmp > 0) {
                Py_DECREF(maxval);
                Py_DECREF(maxitem);
                maxval = val;
                maxitem = item;
            }
            else {
                Py_DECREF(item);
                Py_DECREF(val);
            }
        }
    }
    if (PyErr_Occurred())
        goto Fail_it;
    if (maxval == NULL) {
        assert(maxitem == NULL);
        if (defaultval != NULL) {
            Py_INCREF(defaultval);
            maxitem = defaultval;
        } else {
            PyErr_Format(PyExc_ValueError,
                         "%s() arg is an empty sequence", name);
        }
    }
    else
        Py_DECREF(maxval);
    Py_DECREF(it);
    return maxitem;

Fail_it_item_and_val:
    Py_DECREF(val);
Fail_it_item:
    Py_DECREF(item);
Fail_it:
    Py_XDECREF(maxval);
    Py_XDECREF(maxitem);
    Py_DECREF(it);
    return NULL;
}