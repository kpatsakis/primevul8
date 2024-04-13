static PyTypeObject* make_type(char *type, PyTypeObject* base, char**fields, int num_fields)
{
    _Py_IDENTIFIER(__module__);
    _Py_IDENTIFIER(_ast);
    PyObject *fnames, *result;
    int i;
    fnames = PyTuple_New(num_fields);
    if (!fnames) return NULL;
    for (i = 0; i < num_fields; i++) {
        PyObject *field = PyUnicode_FromString(fields[i]);
        if (!field) {
            Py_DECREF(fnames);
            return NULL;
        }
        PyTuple_SET_ITEM(fnames, i, field);
    }
    result = PyObject_CallFunction((PyObject*)&PyType_Type, "s(O){OOOO}",
                    type, base,
                    _PyUnicode_FromId(&PyId__fields), fnames,
                    _PyUnicode_FromId(&PyId___module__),
                    _PyUnicode_FromId(&PyId__ast));
    Py_DECREF(fnames);
    return (PyTypeObject*)result;
}