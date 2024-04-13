obj2ast_type_ignore(PyObject* obj, type_ignore_ty* out, PyArena* arena)
{
    int isinstance;

    PyObject *tmp = NULL;

    if (obj == Py_None) {
        *out = NULL;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)TypeIgnore_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        int lineno;

        if (_PyObject_LookupAttrId(obj, &PyId_lineno, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"lineno\" missing from TypeIgnore");
            return 1;
        }
        else {
            int res;
            res = obj2ast_int(tmp, &lineno, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = TypeIgnore(lineno, arena);
        if (*out == NULL) goto failed;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "expected some sort of type_ignore, but got %R", obj);
    failed:
    Py_XDECREF(tmp);
    return 1;
}