obj2ast_arguments(PyObject* obj, arguments_ty* out, PyArena* arena)
{
    PyObject* tmp = NULL;
    asdl_seq* args;
    arg_ty vararg;
    asdl_seq* kwonlyargs;
    asdl_seq* kw_defaults;
    arg_ty kwarg;
    asdl_seq* defaults;

    if (lookup_attr_id(obj, &PyId_args, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"args\" missing from arguments");
        return 1;
    }
    else {
        int res;
        Py_ssize_t len;
        Py_ssize_t i;
        if (!PyList_Check(tmp)) {
            PyErr_Format(PyExc_TypeError, "arguments field \"args\" must be a list, not a %.200s", tmp->ob_type->tp_name);
            goto failed;
        }
        len = PyList_GET_SIZE(tmp);
        args = _Ta3_asdl_seq_new(len, arena);
        if (args == NULL) goto failed;
        for (i = 0; i < len; i++) {
            arg_ty val;
            res = obj2ast_arg(PyList_GET_ITEM(tmp, i), &val, arena);
            if (res != 0) goto failed;
            if (len != PyList_GET_SIZE(tmp)) {
                PyErr_SetString(PyExc_RuntimeError, "arguments field \"args\" changed size during iteration");
                goto failed;
            }
            asdl_seq_SET(args, i, val);
        }
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_vararg, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL || tmp == Py_None) {
        Py_CLEAR(tmp);
        vararg = NULL;
    }
    else {
        int res;
        res = obj2ast_arg(tmp, &vararg, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_kwonlyargs, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"kwonlyargs\" missing from arguments");
        return 1;
    }
    else {
        int res;
        Py_ssize_t len;
        Py_ssize_t i;
        if (!PyList_Check(tmp)) {
            PyErr_Format(PyExc_TypeError, "arguments field \"kwonlyargs\" must be a list, not a %.200s", tmp->ob_type->tp_name);
            goto failed;
        }
        len = PyList_GET_SIZE(tmp);
        kwonlyargs = _Ta3_asdl_seq_new(len, arena);
        if (kwonlyargs == NULL) goto failed;
        for (i = 0; i < len; i++) {
            arg_ty val;
            res = obj2ast_arg(PyList_GET_ITEM(tmp, i), &val, arena);
            if (res != 0) goto failed;
            if (len != PyList_GET_SIZE(tmp)) {
                PyErr_SetString(PyExc_RuntimeError, "arguments field \"kwonlyargs\" changed size during iteration");
                goto failed;
            }
            asdl_seq_SET(kwonlyargs, i, val);
        }
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_kw_defaults, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"kw_defaults\" missing from arguments");
        return 1;
    }
    else {
        int res;
        Py_ssize_t len;
        Py_ssize_t i;
        if (!PyList_Check(tmp)) {
            PyErr_Format(PyExc_TypeError, "arguments field \"kw_defaults\" must be a list, not a %.200s", tmp->ob_type->tp_name);
            goto failed;
        }
        len = PyList_GET_SIZE(tmp);
        kw_defaults = _Ta3_asdl_seq_new(len, arena);
        if (kw_defaults == NULL) goto failed;
        for (i = 0; i < len; i++) {
            expr_ty val;
            res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
            if (res != 0) goto failed;
            if (len != PyList_GET_SIZE(tmp)) {
                PyErr_SetString(PyExc_RuntimeError, "arguments field \"kw_defaults\" changed size during iteration");
                goto failed;
            }
            asdl_seq_SET(kw_defaults, i, val);
        }
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_kwarg, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL || tmp == Py_None) {
        Py_CLEAR(tmp);
        kwarg = NULL;
    }
    else {
        int res;
        res = obj2ast_arg(tmp, &kwarg, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (lookup_attr_id(obj, &PyId_defaults, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"defaults\" missing from arguments");
        return 1;
    }
    else {
        int res;
        Py_ssize_t len;
        Py_ssize_t i;
        if (!PyList_Check(tmp)) {
            PyErr_Format(PyExc_TypeError, "arguments field \"defaults\" must be a list, not a %.200s", tmp->ob_type->tp_name);
            goto failed;
        }
        len = PyList_GET_SIZE(tmp);
        defaults = _Ta3_asdl_seq_new(len, arena);
        if (defaults == NULL) goto failed;
        for (i = 0; i < len; i++) {
            expr_ty val;
            res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
            if (res != 0) goto failed;
            if (len != PyList_GET_SIZE(tmp)) {
                PyErr_SetString(PyExc_RuntimeError, "arguments field \"defaults\" changed size during iteration");
                goto failed;
            }
            asdl_seq_SET(defaults, i, val);
        }
        Py_CLEAR(tmp);
    }
    *out = arguments(args, vararg, kwonlyargs, kw_defaults, kwarg, defaults,
                     arena);
    return 0;
failed:
    Py_XDECREF(tmp);
    return 1;
}