parser_sizeof(PyST_Object *st, void *unused)
{
    Py_ssize_t res;

    res = _PyObject_SIZE(Py_TYPE(st)) + _PyNode_SizeOf(st->st_node);
    return PyLong_FromSsize_t(res);
}