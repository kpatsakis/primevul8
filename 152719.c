static int obj2ast_int(PyObject* obj, int* out, PyArena* arena)
{
    int i;
    if (!PyLong_Check(obj)) {
        PyErr_Format(PyExc_ValueError, "invalid integer value: %R", obj);
        return 1;
    }

    i = _PyLong_AsInt(obj);
    if (i == -1 && PyErr_Occurred())
        return 1;
    *out = i;
    return 0;
}