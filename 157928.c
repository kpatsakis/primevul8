obj2ast_expr_context(PyObject* obj, expr_context_ty* out, PyArena* arena)
{
    int isinstance;

    isinstance = PyObject_IsInstance(obj, (PyObject *)Load_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Load;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Store_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Store;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Del_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Del;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)AugLoad_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = AugLoad;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)AugStore_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = AugStore;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Param_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Param;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)NamedStore_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = NamedStore;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "expected some sort of expr_context, but got %R", obj);
    return 1;
}