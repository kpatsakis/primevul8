new_identifier(const char *n, struct compiling *c)
{
    PyObject *id = PyUnicode_DecodeUTF8(n, strlen(n), NULL);
    if (!id)
        return NULL;
    /* PyUnicode_DecodeUTF8 should always return a ready string. */
    assert(PyUnicode_IS_READY(id));
    /* Check whether there are non-ASCII characters in the
       identifier; if so, normalize to NFKC. */
    if (!PyUnicode_IS_ASCII(id)) {
        PyObject *id2;
        _Py_IDENTIFIER(NFKC);
        if (!c->c_normalize && !init_normalization(c)) {
            Py_DECREF(id);
            return NULL;
        }
        PyObject *form = _PyUnicode_FromId(&PyId_NFKC);
        if (form == NULL) {
            Py_DECREF(id);
            return NULL;
        }
        PyObject *args[2] = {form, id};
        id2 = _PyObject_FastCall(c->c_normalize, args, 2);
        Py_DECREF(id);
        if (!id2)
            return NULL;
        if (!PyUnicode_Check(id2)) {
            PyErr_Format(PyExc_TypeError,
                         "unicodedata.normalize() must return a string, not "
                         "%.200s",
                         Py_TYPE(id2)->tp_name);
            Py_DECREF(id2);
            return NULL;
        }
        id = id2;
    }
    PyUnicode_InternInPlace(&id);
    if (PyArena_AddPyObject(c->c_arena, id) < 0) {
        Py_DECREF(id);
        return NULL;
    }
    return id;
}