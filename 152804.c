static int obj2ast_bytes(PyObject* obj, PyObject** out, PyArena* arena)
{
    if (!PyBytes_CheckExact(obj)) {
        PyErr_SetString(PyExc_TypeError, "AST bytes must be of type bytes");
        return 1;
    }
    return obj2ast_object(obj, out, arena);
}