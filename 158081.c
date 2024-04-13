parser__pickler(PyObject *self, PyObject *args)
{
    NOTE(ARGUNUSED(self))
    PyObject *result = NULL;
    PyObject *st = NULL;
    PyObject *empty_dict = NULL;

    if (PyArg_ParseTuple(args, "O!:_pickler", &PyST_Type, &st)) {
        PyObject *newargs;
        PyObject *tuple;

        if ((empty_dict = PyDict_New()) == NULL)
            goto finally;
        if ((newargs = Py_BuildValue("Oi", st, 1)) == NULL)
            goto finally;
        tuple = parser_st2tuple((PyST_Object*)NULL, newargs, empty_dict);
        if (tuple != NULL) {
            result = Py_BuildValue("O(O)", pickle_constructor, tuple);
            Py_DECREF(tuple);
        }
        Py_DECREF(newargs);
    }
  finally:
    Py_XDECREF(empty_dict);

    return (result);
}