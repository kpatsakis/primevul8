builtin___import__(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name", "globals", "locals", "fromlist",
                             "level", 0};
    PyObject *name, *globals = NULL, *locals = NULL, *fromlist = NULL;
    int level = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "U|OOOi:__import__",
                    kwlist, &name, &globals, &locals, &fromlist, &level))
        return NULL;
    return PyImport_ImportModuleLevelObject(name, globals, locals,
                                            fromlist, level);
}