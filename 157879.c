parser_st2tuple(PyST_Object *self, PyObject *args, PyObject *kw)
{
    int line_info = 0;
    int col_info = 0;
    PyObject *res = 0;
    int ok;

    static char *keywords[] = {"st", "line_info", "col_info", NULL};

    if (self == NULL || PyModule_Check(self)) {
        ok = PyArg_ParseTupleAndKeywords(args, kw, "O!|pp:st2tuple", keywords,
                                         &PyST_Type, &self, &line_info,
                                         &col_info);
    }
    else
        ok = PyArg_ParseTupleAndKeywords(args, kw, "|pp:totuple", &keywords[1],
                                         &line_info, &col_info);
    if (ok != 0) {
        /*
         *  Convert ST into a tuple representation.  Use Guido's function,
         *  since it's known to work already.
         */
        res = node2tuple(((PyST_Object*)self)->st_node,
                         PyTuple_New, PyTuple_SetItem, line_info, col_info);
    }
    return (res);
}