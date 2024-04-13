builtin_setattr_impl(PyObject *module, PyObject *obj, PyObject *name,
                     PyObject *value)
/*[clinic end generated code: output=dc2ce1d1add9acb4 input=bd2b7ca6875a1899]*/
{
    if (PyObject_SetAttr(obj, name, value) != 0)
        return NULL;
    Py_RETURN_NONE;
}