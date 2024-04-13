static PyObject *Adapter_ssl_var_lookup(AdapterObject *self, PyObject *args)
{
    APR_OPTIONAL_FN_TYPE(ssl_var_lookup) *ssl_var_lookup = 0;

    PyObject *item = NULL;
    PyObject *latin_item = NULL;

    char *name = 0;
    char *value = 0;

    if (!self->r) {
        PyErr_SetString(PyExc_RuntimeError, "request object has expired");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "O:ssl_var_lookup", &item))
        return NULL;

#if PY_MAJOR_VERSION >= 3
    if (PyUnicode_Check(item)) {
        latin_item = PyUnicode_AsLatin1String(item);
        if (!latin_item) {
            PyErr_Format(PyExc_TypeError, "byte string value expected, "
                         "value containing non 'latin-1' characters found");

            return NULL;
        }

        item = latin_item;
    }
#endif

    if (!PyString_Check(item)) {
        PyErr_Format(PyExc_TypeError, "byte string value expected, value "
                     "of type %.200s found", item->ob_type->tp_name);

        Py_XDECREF(latin_item);

        return NULL;
    }

    name = PyString_AsString(item);

    ssl_var_lookup = APR_RETRIEVE_OPTIONAL_FN(ssl_var_lookup);

    if (ssl_var_lookup == 0)
    {
        Py_XDECREF(latin_item);

        Py_INCREF(Py_None);

        return Py_None;
    }

    value = ssl_var_lookup(self->r->pool, self->r->server,
                           self->r->connection, self->r, name);

    Py_XDECREF(latin_item);

    if (!value) {
        Py_INCREF(Py_None);

        return Py_None;
    }

#if PY_MAJOR_VERSION >= 3
    return PyUnicode_DecodeLatin1(value, strlen(value), NULL);
#else
    return PyString_FromString(value);
#endif
}