builtin_ord(PyObject *module, PyObject *c)
/*[clinic end generated code: output=4fa5e87a323bae71 input=3064e5d6203ad012]*/
{
    long ord;
    Py_ssize_t size;

    if (PyBytes_Check(c)) {
        size = PyBytes_GET_SIZE(c);
        if (size == 1) {
            ord = (long)((unsigned char)*PyBytes_AS_STRING(c));
            return PyLong_FromLong(ord);
        }
    }
    else if (PyUnicode_Check(c)) {
        if (PyUnicode_READY(c) == -1)
            return NULL;
        size = PyUnicode_GET_LENGTH(c);
        if (size == 1) {
            ord = (long)PyUnicode_READ_CHAR(c, 0);
            return PyLong_FromLong(ord);
        }
    }
    else if (PyByteArray_Check(c)) {
        /* XXX Hopefully this is temporary */
        size = PyByteArray_GET_SIZE(c);
        if (size == 1) {
            ord = (long)((unsigned char)*PyByteArray_AS_STRING(c));
            return PyLong_FromLong(ord);
        }
    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "ord() expected string of length 1, but " \
                     "%.200s found", c->ob_type->tp_name);
        return NULL;
    }

    PyErr_Format(PyExc_TypeError,
                 "ord() expected a character, "
                 "but string of length %zd found",
                 size);
    return NULL;
}