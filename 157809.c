parser_compilest(PyST_Object *self, PyObject *args, PyObject *kw)
{
    PyObject*     res = NULL;
    PyArena*      arena = NULL;
    mod_ty        mod;
    PyObject*     filename = NULL;
    int ok;

    static char *keywords[] = {"st", "filename", NULL};

    if (self == NULL || PyModule_Check(self))
        ok = PyArg_ParseTupleAndKeywords(args, kw, "O!|O&:compilest", keywords,
                                         &PyST_Type, &self,
                                         PyUnicode_FSDecoder, &filename);
    else
        ok = PyArg_ParseTupleAndKeywords(args, kw, "|O&:compile", &keywords[1],
                                         PyUnicode_FSDecoder, &filename);
    if (!ok)
        goto error;

    if (filename == NULL) {
        filename = PyUnicode_FromString("<syntax-tree>");
        if (filename == NULL)
            goto error;
    }

    arena = PyArena_New();
    if (!arena)
        goto error;

    mod = PyAST_FromNodeObject(self->st_node, &self->st_flags,
                               filename, arena);
    if (!mod)
        goto error;

    res = (PyObject *)PyAST_CompileObject(mod, filename,
                                          &self->st_flags, -1, arena);
error:
    Py_XDECREF(filename);
    if (arena != NULL)
        PyArena_Free(arena);
    return res;
}