int PyAST_Check(PyObject* obj)
{
    if (!init_types())
        return -1;
    return PyObject_IsInstance(obj, (PyObject*)&AST_type);
}