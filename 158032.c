parser_issuite(PyST_Object *self, PyObject *args, PyObject *kw)
{
    PyObject* res = 0;
    int ok;

    static char *keywords[] = {"st", NULL};

    if (self == NULL || PyModule_Check(self))
        ok = PyArg_ParseTupleAndKeywords(args, kw, "O!:issuite", keywords,
                                         &PyST_Type, &self);
    else
        ok = PyArg_ParseTupleAndKeywords(args, kw, ":issuite", &keywords[1]);

    if (ok) {
        /* Check to see if the ST represents an expression or not. */
        res = (self->st_type == PyST_EXPR) ? Py_False : Py_True;
        Py_INCREF(res);
    }
    return (res);
}