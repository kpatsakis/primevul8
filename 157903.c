parser_do_parse(PyObject *args, PyObject *kw, const char *argspec, int type)
{
    char*     string = 0;
    PyObject* res    = 0;
    int flags        = 0;
    perrdetail err;

    static char *keywords[] = {"source", NULL};

    if (PyArg_ParseTupleAndKeywords(args, kw, argspec, keywords, &string)) {
        node* n = PyParser_ParseStringFlagsFilenameEx(string, NULL,
                                                       &_PyParser_Grammar,
                                                      (type == PyST_EXPR)
                                                      ? eval_input : file_input,
                                                      &err, &flags);

        if (n) {
            res = parser_newstobject(n, type);
            if (res)
                ((PyST_Object *)res)->st_flags.cf_flags = flags & PyCF_MASK;
        }
        else {
            PyParser_SetError(&err);
        }
        PyParser_ClearError(&err);
    }
    return (res);
}