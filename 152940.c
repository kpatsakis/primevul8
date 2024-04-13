static int lookup_attr_id(PyObject *v, _Py_Identifier *name, PyObject **result)
{
    PyObject *oname = _PyUnicode_FromId(name); /* borrowed */
    if (!oname) {
        *result = NULL;
        return -1;
    }
    *result = PyObject_GetAttr(v, oname);
    if (*result == NULL) {
        if (!PyErr_ExceptionMatches(PyExc_AttributeError)) {
            return -1;
        }
        PyErr_Clear();
    }
    return 0;
}