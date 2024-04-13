obj2ast_withitem(PyObject* obj, withitem_ty* out, PyArena* arena)
{
    PyObject* tmp = NULL;
    expr_ty context_expr;
    expr_ty optional_vars;

    if (lookup_attr_id(obj, &PyId_context_expr, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"context_expr\" missing from withitem");
        return 1;
    }
    else {
        int res;
        res = obj2ast_expr(tmp, &context_expr, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_optional_vars, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL || tmp == Py_None) {
        Py_CLEAR(tmp);
        optional_vars = NULL;
    }
    else {
        int res;
        res = obj2ast_expr(tmp, &optional_vars, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    *out = withitem(context_expr, optional_vars, arena);
    return 0;
failed:
    Py_XDECREF(tmp);
    return 1;
}