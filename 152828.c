obj2ast_boolop(PyObject* obj, boolop_ty* out, PyArena* arena)
{
    int isinstance;

    isinstance = PyObject_IsInstance(obj, (PyObject *)And_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = And;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Or_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Or;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "expected some sort of boolop, but got %R", obj);
    return 1;
}