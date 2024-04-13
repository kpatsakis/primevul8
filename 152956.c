static int obj2ast_constant(PyObject* obj, PyObject** out, PyArena* arena)
{
    if (obj) {
        if (PyArena_AddPyObject(arena, obj) < 0) {
            *out = NULL;
            return -1;
        }
        Py_INCREF(obj);
    }
    *out = obj;
    return 0;
}