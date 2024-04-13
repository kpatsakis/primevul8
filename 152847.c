obj2ast_cmpop(PyObject* obj, cmpop_ty* out, PyArena* arena)
{
    int isinstance;

    isinstance = PyObject_IsInstance(obj, (PyObject *)Eq_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Eq;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)NotEq_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = NotEq;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Lt_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Lt;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)LtE_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = LtE;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Gt_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Gt;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)GtE_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = GtE;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Is_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Is;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)IsNot_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = IsNot;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)In_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = In;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)NotIn_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = NotIn;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "expected some sort of cmpop, but got %R", obj);
    return 1;
}