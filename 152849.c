ast_warn(struct compiling *c, const node *n, char *msg)
{
    if (PyErr_WarnExplicit(PyExc_SyntaxWarning, msg, c->c_filename, LINENO(n),
                           NULL, NULL) < 0) {
        /* if -Werr, change it to a SyntaxError */
        if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_SyntaxWarning))
            ast_error(n, msg);
        return 0;
    }
    return 1;
}