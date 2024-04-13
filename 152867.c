static PyObject* ast2obj_object(void *o)
{
    if (!o)
        o = Py_None;
    Py_INCREF((PyObject*)o);
    return (PyObject*)o;
}