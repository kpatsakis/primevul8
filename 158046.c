obj2ast_mod(PyObject* obj, mod_ty* out, PyArena* arena)
{
    int isinstance;

    PyObject *tmp = NULL;

    if (obj == Py_None) {
        *out = NULL;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Module_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* body;
        asdl_seq* type_ignores;

        if (_PyObject_LookupAttrId(obj, &PyId_body, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"body\" missing from Module");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Module field \"body\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            body = _Py_asdl_seq_new(len, arena);
            if (body == NULL) goto failed;
            for (i = 0; i < len; i++) {
                stmt_ty val;
                res = obj2ast_stmt(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Module field \"body\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(body, i, val);
            }
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_type_ignores, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"type_ignores\" missing from Module");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Module field \"type_ignores\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            type_ignores = _Py_asdl_seq_new(len, arena);
            if (type_ignores == NULL) goto failed;
            for (i = 0; i < len; i++) {
                type_ignore_ty val;
                res = obj2ast_type_ignore(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Module field \"type_ignores\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(type_ignores, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = Module(body, type_ignores, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Interactive_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* body;

        if (_PyObject_LookupAttrId(obj, &PyId_body, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"body\" missing from Interactive");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Interactive field \"body\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            body = _Py_asdl_seq_new(len, arena);
            if (body == NULL) goto failed;
            for (i = 0; i < len; i++) {
                stmt_ty val;
                res = obj2ast_stmt(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Interactive field \"body\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(body, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = Interactive(body, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Expression_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty body;

        if (_PyObject_LookupAttrId(obj, &PyId_body, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"body\" missing from Expression");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &body, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Expression(body, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)FunctionType_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* argtypes;
        expr_ty returns;

        if (_PyObject_LookupAttrId(obj, &PyId_argtypes, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"argtypes\" missing from FunctionType");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "FunctionType field \"argtypes\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            argtypes = _Py_asdl_seq_new(len, arena);
            if (argtypes == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "FunctionType field \"argtypes\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(argtypes, i, val);
            }
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_returns, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"returns\" missing from FunctionType");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &returns, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = FunctionType(argtypes, returns, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Suite_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* body;

        if (_PyObject_LookupAttrId(obj, &PyId_body, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"body\" missing from Suite");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Suite field \"body\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            body = _Py_asdl_seq_new(len, arena);
            if (body == NULL) goto failed;
            for (i = 0; i < len; i++) {
                stmt_ty val;
                res = obj2ast_stmt(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Suite field \"body\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(body, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = Suite(body, arena);
        if (*out == NULL) goto failed;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "expected some sort of mod, but got %R", obj);
    failed:
    Py_XDECREF(tmp);
    return 1;
}