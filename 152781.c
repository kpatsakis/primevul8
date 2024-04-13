static int obj2ast_string(PyObject* obj, PyObject** out, PyArena* arena)
{
    if (!PyUnicode_CheckExact(obj) && !PyBytes_CheckExact(obj)) {
        PyErr_SetString(PyExc_TypeError, "AST string must be of type str");
        return 1;
    }
    return obj2ast_object(obj, out, arena);
}