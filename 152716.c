obj2ast_alias(PyObject* obj, alias_ty* out, PyArena* arena)
{
    PyObject* tmp = NULL;
    identifier name;
    identifier asname;

    if (lookup_attr_id(obj, &PyId_name, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"name\" missing from alias");
        return 1;
    }
    else {
        int res;
        res = obj2ast_identifier(tmp, &name, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_asname, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL || tmp == Py_None) {
        Py_CLEAR(tmp);
        asname = NULL;
    }
    else {
        int res;
        res = obj2ast_identifier(tmp, &asname, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    *out = alias(name, asname, arena);
    return 0;
failed:
    Py_XDECREF(tmp);
    return 1;
}