node2tuple(node *n,                     /* node to convert               */
           SeqMaker mkseq,              /* create sequence               */
           SeqInserter addelem,         /* func. to add elem. in seq.    */
           int lineno,                  /* include line numbers?         */
           int col_offset)              /* include column offsets?       */
{
    PyObject *result = NULL, *w;

    if (n == NULL) {
        Py_RETURN_NONE;
    }

    if (ISNONTERMINAL(TYPE(n))) {
        int i;

        result = mkseq(1 + NCH(n) + (TYPE(n) == encoding_decl));
        if (result == NULL)
            goto error;

        w = PyLong_FromLong(TYPE(n));
        if (w == NULL)
            goto error;
        (void) addelem(result, 0, w);

        for (i = 0; i < NCH(n); i++) {
            w = node2tuple(CHILD(n, i), mkseq, addelem, lineno, col_offset);
            if (w == NULL)
                goto error;
            (void) addelem(result, i+1, w);
        }

        if (TYPE(n) == encoding_decl) {
            w = PyUnicode_FromString(STR(n));
            if (w == NULL)
                goto error;
            (void) addelem(result, i+1, w);
        }
    }
    else if (ISTERMINAL(TYPE(n))) {
        result = mkseq(2 + lineno + col_offset);
        if (result == NULL)
            goto error;

        w = PyLong_FromLong(TYPE(n));
        if (w == NULL)
            goto error;
        (void) addelem(result, 0, w);

        w = PyUnicode_FromString(STR(n));
        if (w == NULL)
            goto error;
        (void) addelem(result, 1, w);

        if (lineno) {
            w = PyLong_FromLong(n->n_lineno);
            if (w == NULL)
                goto error;
            (void) addelem(result, 2, w);
        }

        if (col_offset) {
            w = PyLong_FromLong(n->n_col_offset);
            if (w == NULL)
                goto error;
            (void) addelem(result, 2 + lineno, w);
        }
    }
    else {
        PyErr_SetString(PyExc_SystemError,
                        "unrecognized parse tree node type");
        return ((PyObject*) NULL);
    }
    return result;

error:
    Py_XDECREF(result);
    return NULL;
}