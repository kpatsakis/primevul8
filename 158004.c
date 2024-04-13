PyRun_FileExFlags(FILE *fp, const char *filename_str, int start, PyObject *globals,
                  PyObject *locals, int closeit, PyCompilerFlags *flags)
{
    PyObject *ret = NULL;
    mod_ty mod;
    PyArena *arena = NULL;
    PyObject *filename;

    filename = PyUnicode_DecodeFSDefault(filename_str);
    if (filename == NULL)
        goto exit;

    arena = PyArena_New();
    if (arena == NULL)
        goto exit;

    mod = PyParser_ASTFromFileObject(fp, filename, NULL, start, 0, 0,
                                     flags, NULL, arena);
    if (closeit)
        fclose(fp);
    if (mod == NULL) {
        goto exit;
    }
    ret = run_mod(mod, filename, globals, locals, flags, arena);

exit:
    Py_XDECREF(filename);
    if (arena != NULL)
        PyArena_Free(arena);
    return ret;
}