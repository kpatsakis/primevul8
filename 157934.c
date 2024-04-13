PyParser_ASTFromStringObject(const char *s, PyObject *filename, int start,
                             PyCompilerFlags *flags, PyArena *arena)
{
    mod_ty mod;
    PyCompilerFlags localflags;
    perrdetail err;
    int iflags = PARSER_FLAGS(flags);

    node *n = PyParser_ParseStringObject(s, filename,
                                         &_PyParser_Grammar, start, &err,
                                         &iflags);
    if (flags == NULL) {
        localflags.cf_flags = 0;
        flags = &localflags;
    }
    if (n) {
        flags->cf_flags |= iflags & PyCF_MASK;
        mod = PyAST_FromNodeObject(n, flags, filename, arena);
        PyNode_Free(n);
    }
    else {
        err_input(&err);
        mod = NULL;
    }
    err_free(&err);
    return mod;
}