obj2ast_unaryop(PyObject* obj, unaryop_ty* out, PyArena* arena)
{
    int isinstance;

    isinstance = PyObject_IsInstance(obj, (PyObject *)Invert_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Invert;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Not_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Not;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)UAdd_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = UAdd;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)USub_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = USub;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "expected some sort of unaryop, but got %R", obj);
    return 1;
}