static int obj2ast_identifier(PyObject* obj, PyObject** out, PyArena* arena)
{
    if (!PyUnicode_CheckExact(obj) && obj != Py_None) {
        PyErr_SetString(PyExc_TypeError, "AST identifier must be of type str");
        return 1;
    }
    return obj2ast_object(obj, out, arena);
}