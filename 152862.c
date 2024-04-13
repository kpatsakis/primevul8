obj2ast_slice(PyObject* obj, slice_ty* out, PyArena* arena)
{
    int isinstance;

    PyObject *tmp = NULL;

    if (obj == Py_None) {
        *out = NULL;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Slice_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty lower;
        expr_ty upper;
        expr_ty step;

        if (lookup_attr_id(obj, &PyId_lower, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL || tmp == Py_None) {
            Py_CLEAR(tmp);
            lower = NULL;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &lower, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (lookup_attr_id(obj, &PyId_upper, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL || tmp == Py_None) {
            Py_CLEAR(tmp);
            upper = NULL;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &upper, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (lookup_attr_id(obj, &PyId_step, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL || tmp == Py_None) {
            Py_CLEAR(tmp);
            step = NULL;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &step, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Slice(lower, upper, step, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)ExtSlice_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* dims;

        if (lookup_attr_id(obj, &PyId_dims, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"dims\" missing from ExtSlice");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "ExtSlice field \"dims\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            dims = _Ta3_asdl_seq_new(len, arena);
            if (dims == NULL) goto failed;
            for (i = 0; i < len; i++) {
                slice_ty val;
                res = obj2ast_slice(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "ExtSlice field \"dims\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(dims, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = ExtSlice(dims, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Index_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty value;

        if (lookup_attr_id(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from Index");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Index(value, arena);
        if (*out == NULL) goto failed;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "expected some sort of slice, but got %R", obj);
    failed:
    Py_XDECREF(tmp);
    return 1;
}