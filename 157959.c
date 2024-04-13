obj2ast_expr(PyObject* obj, expr_ty* out, PyArena* arena)
{
    int isinstance;

    PyObject *tmp = NULL;
    int lineno;
    int col_offset;
    int end_lineno;
    int end_col_offset;

    if (obj == Py_None) {
        *out = NULL;
        return 0;
    }
    if (_PyObject_LookupAttrId(obj, &PyId_lineno, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"lineno\" missing from expr");
        return 1;
    }
    else {
        int res;
        res = obj2ast_int(tmp, &lineno, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (_PyObject_LookupAttrId(obj, &PyId_col_offset, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL) {
        PyErr_SetString(PyExc_TypeError, "required field \"col_offset\" missing from expr");
        return 1;
    }
    else {
        int res;
        res = obj2ast_int(tmp, &col_offset, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (_PyObject_LookupAttrId(obj, &PyId_end_lineno, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL || tmp == Py_None) {
        Py_CLEAR(tmp);
        end_lineno = 0;
    }
    else {
        int res;
        res = obj2ast_int(tmp, &end_lineno, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    if (_PyObject_LookupAttrId(obj, &PyId_end_col_offset, &tmp) < 0) {
        return 1;
    }
    if (tmp == NULL || tmp == Py_None) {
        Py_CLEAR(tmp);
        end_col_offset = 0;
    }
    else {
        int res;
        res = obj2ast_int(tmp, &end_col_offset, arena);
        if (res != 0) goto failed;
        Py_CLEAR(tmp);
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)BoolOp_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        boolop_ty op;
        asdl_seq* values;

        if (_PyObject_LookupAttrId(obj, &PyId_op, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"op\" missing from BoolOp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_boolop(tmp, &op, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_values, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"values\" missing from BoolOp");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "BoolOp field \"values\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            values = _Py_asdl_seq_new(len, arena);
            if (values == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "BoolOp field \"values\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(values, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = BoolOp(op, values, lineno, col_offset, end_lineno,
                      end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)NamedExpr_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty target;
        expr_ty value;

        if (_PyObject_LookupAttrId(obj, &PyId_target, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"target\" missing from NamedExpr");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &target, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from NamedExpr");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = NamedExpr(target, value, lineno, col_offset, end_lineno,
                         end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)BinOp_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty left;
        operator_ty op;
        expr_ty right;

        if (_PyObject_LookupAttrId(obj, &PyId_left, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"left\" missing from BinOp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &left, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_op, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"op\" missing from BinOp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_operator(tmp, &op, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_right, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"right\" missing from BinOp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &right, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = BinOp(left, op, right, lineno, col_offset, end_lineno,
                     end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)UnaryOp_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        unaryop_ty op;
        expr_ty operand;

        if (_PyObject_LookupAttrId(obj, &PyId_op, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"op\" missing from UnaryOp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_unaryop(tmp, &op, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_operand, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"operand\" missing from UnaryOp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &operand, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = UnaryOp(op, operand, lineno, col_offset, end_lineno,
                       end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Lambda_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        arguments_ty args;
        expr_ty body;

        if (_PyObject_LookupAttrId(obj, &PyId_args, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"args\" missing from Lambda");
            return 1;
        }
        else {
            int res;
            res = obj2ast_arguments(tmp, &args, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_body, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"body\" missing from Lambda");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &body, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Lambda(args, body, lineno, col_offset, end_lineno,
                      end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)IfExp_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty test;
        expr_ty body;
        expr_ty orelse;

        if (_PyObject_LookupAttrId(obj, &PyId_test, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"test\" missing from IfExp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &test, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_body, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"body\" missing from IfExp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &body, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_orelse, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"orelse\" missing from IfExp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &orelse, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = IfExp(test, body, orelse, lineno, col_offset, end_lineno,
                     end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Dict_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* keys;
        asdl_seq* values;

        if (_PyObject_LookupAttrId(obj, &PyId_keys, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"keys\" missing from Dict");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Dict field \"keys\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            keys = _Py_asdl_seq_new(len, arena);
            if (keys == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Dict field \"keys\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(keys, i, val);
            }
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_values, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"values\" missing from Dict");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Dict field \"values\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            values = _Py_asdl_seq_new(len, arena);
            if (values == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Dict field \"values\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(values, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = Dict(keys, values, lineno, col_offset, end_lineno,
                    end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Set_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* elts;

        if (_PyObject_LookupAttrId(obj, &PyId_elts, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"elts\" missing from Set");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Set field \"elts\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            elts = _Py_asdl_seq_new(len, arena);
            if (elts == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Set field \"elts\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(elts, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = Set(elts, lineno, col_offset, end_lineno, end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)ListComp_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty elt;
        asdl_seq* generators;

        if (_PyObject_LookupAttrId(obj, &PyId_elt, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"elt\" missing from ListComp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &elt, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_generators, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"generators\" missing from ListComp");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "ListComp field \"generators\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            generators = _Py_asdl_seq_new(len, arena);
            if (generators == NULL) goto failed;
            for (i = 0; i < len; i++) {
                comprehension_ty val;
                res = obj2ast_comprehension(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "ListComp field \"generators\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(generators, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = ListComp(elt, generators, lineno, col_offset, end_lineno,
                        end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)SetComp_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty elt;
        asdl_seq* generators;

        if (_PyObject_LookupAttrId(obj, &PyId_elt, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"elt\" missing from SetComp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &elt, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_generators, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"generators\" missing from SetComp");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "SetComp field \"generators\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            generators = _Py_asdl_seq_new(len, arena);
            if (generators == NULL) goto failed;
            for (i = 0; i < len; i++) {
                comprehension_ty val;
                res = obj2ast_comprehension(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "SetComp field \"generators\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(generators, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = SetComp(elt, generators, lineno, col_offset, end_lineno,
                       end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)DictComp_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty key;
        expr_ty value;
        asdl_seq* generators;

        if (_PyObject_LookupAttrId(obj, &PyId_key, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"key\" missing from DictComp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &key, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from DictComp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_generators, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"generators\" missing from DictComp");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "DictComp field \"generators\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            generators = _Py_asdl_seq_new(len, arena);
            if (generators == NULL) goto failed;
            for (i = 0; i < len; i++) {
                comprehension_ty val;
                res = obj2ast_comprehension(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "DictComp field \"generators\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(generators, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = DictComp(key, value, generators, lineno, col_offset, end_lineno,
                        end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)GeneratorExp_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty elt;
        asdl_seq* generators;

        if (_PyObject_LookupAttrId(obj, &PyId_elt, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"elt\" missing from GeneratorExp");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &elt, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_generators, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"generators\" missing from GeneratorExp");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "GeneratorExp field \"generators\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            generators = _Py_asdl_seq_new(len, arena);
            if (generators == NULL) goto failed;
            for (i = 0; i < len; i++) {
                comprehension_ty val;
                res = obj2ast_comprehension(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "GeneratorExp field \"generators\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(generators, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = GeneratorExp(elt, generators, lineno, col_offset, end_lineno,
                            end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Await_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty value;

        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from Await");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Await(value, lineno, col_offset, end_lineno, end_col_offset,
                     arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Yield_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty value;

        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL || tmp == Py_None) {
            Py_CLEAR(tmp);
            value = NULL;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Yield(value, lineno, col_offset, end_lineno, end_col_offset,
                     arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)YieldFrom_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty value;

        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from YieldFrom");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = YieldFrom(value, lineno, col_offset, end_lineno, end_col_offset,
                         arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Compare_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty left;
        asdl_int_seq* ops;
        asdl_seq* comparators;

        if (_PyObject_LookupAttrId(obj, &PyId_left, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"left\" missing from Compare");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &left, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_ops, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"ops\" missing from Compare");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Compare field \"ops\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            ops = _Py_asdl_int_seq_new(len, arena);
            if (ops == NULL) goto failed;
            for (i = 0; i < len; i++) {
                cmpop_ty val;
                res = obj2ast_cmpop(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Compare field \"ops\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(ops, i, val);
            }
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_comparators, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"comparators\" missing from Compare");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Compare field \"comparators\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            comparators = _Py_asdl_seq_new(len, arena);
            if (comparators == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Compare field \"comparators\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(comparators, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = Compare(left, ops, comparators, lineno, col_offset, end_lineno,
                       end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Call_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty func;
        asdl_seq* args;
        asdl_seq* keywords;

        if (_PyObject_LookupAttrId(obj, &PyId_func, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"func\" missing from Call");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &func, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_args, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"args\" missing from Call");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Call field \"args\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            args = _Py_asdl_seq_new(len, arena);
            if (args == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Call field \"args\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(args, i, val);
            }
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_keywords, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"keywords\" missing from Call");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Call field \"keywords\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            keywords = _Py_asdl_seq_new(len, arena);
            if (keywords == NULL) goto failed;
            for (i = 0; i < len; i++) {
                keyword_ty val;
                res = obj2ast_keyword(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Call field \"keywords\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(keywords, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = Call(func, args, keywords, lineno, col_offset, end_lineno,
                    end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)FormattedValue_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty value;
        int conversion;
        expr_ty format_spec;

        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from FormattedValue");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_conversion, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL || tmp == Py_None) {
            Py_CLEAR(tmp);
            conversion = 0;
        }
        else {
            int res;
            res = obj2ast_int(tmp, &conversion, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_format_spec, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL || tmp == Py_None) {
            Py_CLEAR(tmp);
            format_spec = NULL;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &format_spec, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = FormattedValue(value, conversion, format_spec, lineno,
                              col_offset, end_lineno, end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)JoinedStr_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* values;

        if (_PyObject_LookupAttrId(obj, &PyId_values, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"values\" missing from JoinedStr");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "JoinedStr field \"values\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            values = _Py_asdl_seq_new(len, arena);
            if (values == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "JoinedStr field \"values\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(values, i, val);
            }
            Py_CLEAR(tmp);
        }
        *out = JoinedStr(values, lineno, col_offset, end_lineno,
                         end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Constant_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        constant value;

        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from Constant");
            return 1;
        }
        else {
            int res;
            res = obj2ast_constant(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Constant(value, lineno, col_offset, end_lineno, end_col_offset,
                        arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Attribute_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty value;
        identifier attr;
        expr_context_ty ctx;

        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from Attribute");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_attr, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"attr\" missing from Attribute");
            return 1;
        }
        else {
            int res;
            res = obj2ast_identifier(tmp, &attr, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_ctx, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"ctx\" missing from Attribute");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr_context(tmp, &ctx, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Attribute(value, attr, ctx, lineno, col_offset, end_lineno,
                         end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Subscript_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty value;
        slice_ty slice;
        expr_context_ty ctx;

        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from Subscript");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_slice, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"slice\" missing from Subscript");
            return 1;
        }
        else {
            int res;
            res = obj2ast_slice(tmp, &slice, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_ctx, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"ctx\" missing from Subscript");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr_context(tmp, &ctx, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Subscript(value, slice, ctx, lineno, col_offset, end_lineno,
                         end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Starred_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        expr_ty value;
        expr_context_ty ctx;

        if (_PyObject_LookupAttrId(obj, &PyId_value, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"value\" missing from Starred");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr(tmp, &value, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_ctx, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"ctx\" missing from Starred");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr_context(tmp, &ctx, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Starred(value, ctx, lineno, col_offset, end_lineno,
                       end_col_offset, arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Name_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        identifier id;
        expr_context_ty ctx;

        if (_PyObject_LookupAttrId(obj, &PyId_id, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"id\" missing from Name");
            return 1;
        }
        else {
            int res;
            res = obj2ast_identifier(tmp, &id, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_ctx, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"ctx\" missing from Name");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr_context(tmp, &ctx, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Name(id, ctx, lineno, col_offset, end_lineno, end_col_offset,
                    arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)List_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* elts;
        expr_context_ty ctx;

        if (_PyObject_LookupAttrId(obj, &PyId_elts, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"elts\" missing from List");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "List field \"elts\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            elts = _Py_asdl_seq_new(len, arena);
            if (elts == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "List field \"elts\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(elts, i, val);
            }
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_ctx, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"ctx\" missing from List");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr_context(tmp, &ctx, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = List(elts, ctx, lineno, col_offset, end_lineno, end_col_offset,
                    arena);
        if (*out == NULL) goto failed;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject*)Tuple_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        asdl_seq* elts;
        expr_context_ty ctx;

        if (_PyObject_LookupAttrId(obj, &PyId_elts, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"elts\" missing from Tuple");
            return 1;
        }
        else {
            int res;
            Py_ssize_t len;
            Py_ssize_t i;
            if (!PyList_Check(tmp)) {
                PyErr_Format(PyExc_TypeError, "Tuple field \"elts\" must be a list, not a %.200s", tmp->ob_type->tp_name);
                goto failed;
            }
            len = PyList_GET_SIZE(tmp);
            elts = _Py_asdl_seq_new(len, arena);
            if (elts == NULL) goto failed;
            for (i = 0; i < len; i++) {
                expr_ty val;
                res = obj2ast_expr(PyList_GET_ITEM(tmp, i), &val, arena);
                if (res != 0) goto failed;
                if (len != PyList_GET_SIZE(tmp)) {
                    PyErr_SetString(PyExc_RuntimeError, "Tuple field \"elts\" changed size during iteration");
                    goto failed;
                }
                asdl_seq_SET(elts, i, val);
            }
            Py_CLEAR(tmp);
        }
        if (_PyObject_LookupAttrId(obj, &PyId_ctx, &tmp) < 0) {
            return 1;
        }
        if (tmp == NULL) {
            PyErr_SetString(PyExc_TypeError, "required field \"ctx\" missing from Tuple");
            return 1;
        }
        else {
            int res;
            res = obj2ast_expr_context(tmp, &ctx, arena);
            if (res != 0) goto failed;
            Py_CLEAR(tmp);
        }
        *out = Tuple(elts, ctx, lineno, col_offset, end_lineno, end_col_offset,
                     arena);
        if (*out == NULL) goto failed;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "expected some sort of expr, but got %R", obj);
    failed:
    Py_XDECREF(tmp);
    return 1;
}