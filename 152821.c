obj2ast_operator(PyObject* obj, operator_ty* out, PyArena* arena)
{
    int isinstance;

    isinstance = PyObject_IsInstance(obj, (PyObject *)Add_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Add;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Sub_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Sub;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Mult_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Mult;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)MatMult_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = MatMult;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Div_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Div;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Mod_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Mod;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)Pow_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = Pow;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)LShift_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = LShift;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)RShift_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = RShift;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)BitOr_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = BitOr;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)BitXor_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = BitXor;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)BitAnd_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = BitAnd;
        return 0;
    }
    isinstance = PyObject_IsInstance(obj, (PyObject *)FloorDiv_type);
    if (isinstance == -1) {
        return 1;
    }
    if (isinstance) {
        *out = FloorDiv;
        return 0;
    }

    PyErr_Format(PyExc_TypeError, "expected some sort of operator, but got %R", obj);
    return 1;
}