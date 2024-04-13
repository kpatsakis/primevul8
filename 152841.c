obj2ast_arg(PyObject* obj, arg_ty* out, PyArena* arena)
{
    PyObject* tmp = NULL;
    identifier arg;
    expr_ty annotation;
    string type_comment;
    int lineno;
    int col_offset;

    if (lookup_attr_id(obj, &PyId_arg, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"arg\" missing from arg");
        return 1;
    }
    else {
        int res;
        res = obj2ast_identifier(tmp, &arg, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_annotation, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL || tmp == Py_None) {
        Py_CLEAR(tmp);
        annotation = NULL;
    }
    else {
        int res;
        res = obj2ast_expr(tmp, &annotation, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_type_comment, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL || tmp == Py_None) {
        Py_CLEAR(tmp);
        type_comment = NULL;
    }
    else {
        int res;
        res = obj2ast_string(tmp, &type_comment, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_lineno, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"lineno\" missing from arg");
        return 1;
    }
    else {
        int res;
        res = obj2ast_int(tmp, &lineno, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_col_offset, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"col_offset\" missing from arg");
        return 1;
    }
    else {
        int res;
        res = obj2ast_int(tmp, &col_offset, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    *out = arg(arg, annotation, type_comment, lineno, col_offset, arena);
    return 0;
failed:
    Py_XDECREF(tmp);
    return 1;
}