ast_type_reduce(PyObject *self, PyObject *unused)
{
    _Py_IDENTIFIER(__dict__);
    PyObject *dict;
    if (_PyObject_LookupAttrId(self, &PyId___dict__, &dict) < 0) {
        return NULL;
    }
    if (dict) {
        return Py_BuildValue("O()N", Py_TYPE(self), dict);
    }
    return Py_BuildValue("O()", Py_TYPE(self));
}