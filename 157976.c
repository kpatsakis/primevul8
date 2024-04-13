PyRun_StringFlags(const char *str, int start, PyObject *globals,
                  PyObject *locals, PyCompilerFlags *flags)
{
    PyObject *ret = NULL;
    mod_ty mod;
    PyArena *arena;
    PyObject *filename;

    filename = _PyUnicode_FromId(&PyId_string); /* borrowed */
    if (filename == NULL)
        return NULL;

    arena = PyArena_New();
    if (arena == NULL)
        return NULL;

    mod = PyParser_ASTFromStringObject(str, filename, start, flags, arena);
    if (mod != NULL)
        ret = run_mod(mod, filename, globals, locals, flags, arena);
    PyArena_Free(arena);
    return ret;
}