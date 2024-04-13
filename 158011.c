builtin_round_impl(PyObject *module, PyObject *number, PyObject *ndigits)
/*[clinic end generated code: output=ff0d9dd176c02ede input=854bc3a217530c3d]*/
{
    PyObject *round, *result;

    if (Py_TYPE(number)->tp_dict == NULL) {
        if (PyType_Ready(Py_TYPE(number)) < 0)
            return NULL;
    }

    round = _PyObject_LookupSpecial(number, &PyId___round__);
    if (round == NULL) {
        if (!PyErr_Occurred())
            PyErr_Format(PyExc_TypeError,
                         "type %.100s doesn't define __round__ method",
                         Py_TYPE(number)->tp_name);
        return NULL;
    }

    if (ndigits == NULL || ndigits == Py_None)
        result = _PyObject_CallNoArg(round);
    else
        result = PyObject_CallFunctionObjArgs(round, ndigits, NULL);
    Py_DECREF(round);
    return result;
}