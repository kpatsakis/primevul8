PyParser_ASTFromFileObject(FILE *fp, PyObject *filename, const char* enc,
                           int start, const char *ps1,
                           const char *ps2, PyCompilerFlags *flags, int *errcode,
                           PyArena *arena)
{
    mod_ty mod;
    PyCompilerFlags localflags;
    perrdetail err;
    int iflags = PARSER_FLAGS(flags);

    node *n = PyParser_ParseFileObject(fp, filename, enc,
                                       &_PyParser_Grammar,
                                       start, ps1, ps2, &err, &iflags);
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
        if (errcode)
            *errcode = err.error;
        mod = NULL;
    }
    err_free(&err);
    return mod;
}