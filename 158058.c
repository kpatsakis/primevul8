parser_isexpr(PyST_Object *self, PyObject *args, PyObject *kw)
{
    PyObject* res = 0;
    int ok;

    static char *keywords[] = {"st", NULL};

    if (self == NULL || PyModule_Check(self))
        ok = PyArg_ParseTupleAndKeywords(args, kw, "O!:isexpr", keywords,
                                         &PyST_Type, &self);
    else
        ok = PyArg_ParseTupleAndKeywords(args, kw, ":isexpr", &keywords[1]);

    if (ok) {
        /* Check to see if the ST represents an expression or not. */
        res = (self->st_type == PyST_EXPR) ? Py_True : Py_False;
        Py_INCREF(res);
    }
    return (res);
}