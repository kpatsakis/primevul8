obj2ast_comprehension(PyObject* obj, comprehension_ty* out, PyArena* arena)
{
    PyObject* tmp = NULL;
    expr_ty target;
    expr_ty iter;
    asdl_seq* ifs;
    int is_async;

    if (lookup_attr_id(obj, &PyId_target, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"target\" missing from comprehension");
        return 1;
    }
    else {
        int res;
        res = obj2ast_expr(tmp, &target, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_iter, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"iter\" missing from comprehension");
        return 1;
    }
    else {
        int res;
        res = obj2ast_expr(tmp, &iter, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_ifs, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"ifs\" missing from comprehension");
        return 1;
    }
    else {
        int res;
        Py_ssize_t len;
        Py_ssize_t i;
        if (!PyList_Check(tmp)) {
            PyErr_Format(PyExc_TypeError, "comprehension field \"ifs\" must be a list, not a %.200s", tmp->ob_type->tp_name);
            goto failed;
        }
        len = PyList_GET_SIZE(tmp);
        ifs = _Ta3_asdl_seq_new(len, arena);
        if (ifs == NULL) goto failed;
        for (i = 0; i < len; i++) {
            expr_ty val;
            res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
            if (res != 0) goto failed;
            if (len != PyList_GET_SIZE(tmp)) {
                PyErr_SetString(PyExc_RuntimeError, "comprehension field \"ifs\" changed size during iteration");
                goto failed;
            }
            asdl_seq_SET(ifs, i, val);
        }
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_is_async, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"is_async\" missing from comprehension");
        return 1;
    }
    else {
        int res;
        res = obj2ast_int(tmp, &is_async, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    *out = comprehension(target, iter, ifs, is_async, arena);
    return 0;
failed:
    Py_XDECREF(tmp);
    return 1;
}