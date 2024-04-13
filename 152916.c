obj2ast_keyword(PyObject* obj, keyword_ty* out, PyArena* arena)
{
    PyObject* tmp = NULL;
    identifier arg;
    expr_ty value;

    if (lookup_attr_id(obj, &PyId_arg, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL || tmp == Py_None) {
        Py_CLEAR(tmp);
        arg = NULL;
    }
    else {
        int res;
        res = obj2ast_identifier(tmp, &arg, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_value, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from keyword");
        return 1;
    }
    else {
        int res;
        res = obj2ast_expr(tmp, &value, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    *out = keyword(arg, value, arena);
    return 0;
failed:
    Py_XDECREF(tmp);
    return 1;
}