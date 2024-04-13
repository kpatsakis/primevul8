builtin_next(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
    PyObject *it, *res;

    if (!_PyArg_CheckPositional("next", nargs, 1, 2))
        return NULL;

    it = args[0];
    if (!PyIter_Check(it)) {
        PyErr_Format(PyExc_TypeError,
            "'%.200s' object is not an iterator",
            it->ob_type->tp_name);
        return NULL;
    }

    res = (*it->ob_type->tp_iternext)(it);
    if (res != NULL) {
        return res;
    } else if (nargs > 1) {
        PyObject *def = args[1];
        if (PyErr_Occurred()) {
            if(!PyErr_ExceptionMatches(PyExc_StopIteration))
                return NULL;
            PyErr_Clear();
        }
        Py_INCREF(def);
        return def;
    } else if (PyErr_Occurred()) {
        return NULL;
    } else {
        PyErr_SetNone(PyExc_StopIteration);
        return NULL;
    }
}