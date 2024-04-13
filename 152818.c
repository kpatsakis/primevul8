static int obj2ast_singleton(PyObject *obj, PyObject** out, PyArena* arena)
{
    if (obj != Py_None && obj != Py_True && obj != Py_False) {
        PyErr_SetString(PyExc_ValueError,
                        "AST singleton must be True, False, or None");
        return 1;
    }
    *out = obj;
    return 0;
}