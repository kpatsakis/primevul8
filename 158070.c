builtin_delattr_impl(PyObject *module, PyObject *obj, PyObject *name)
/*[clinic end generated code: output=85134bc58dff79fa input=db16685d6b4b9410]*/
{
    if (PyObject_SetAttr(obj, name, (PyObject *)NULL) != 0)
        return NULL;
    Py_RETURN_NONE;
}