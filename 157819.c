builtin_hasattr_impl(PyObject *module, PyObject *obj, PyObject *name)
/*[clinic end generated code: output=a7aff2090a4151e5 input=0faec9787d979542]*/
{
    PyObject *v;

    if (!PyUnicode_Check(name)) {
        PyErr_SetString(PyExc_TypeError,
                        "hasattr(): attribute name must be string");
        return NULL;
    }
    if (_PyObject_LookupAttr(obj, name, &v) < 0) {
        return NULL;
    }
    if (v == NULL) {
        Py_RETURN_FALSE;
    }
    Py_DECREF(v);
    Py_RETURN_TRUE;
}