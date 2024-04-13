Py_SymtableStringObject(const char *str, PyObject *filename, int start)
{
    struct symtable *st;
    mod_ty mod;
    PyCompilerFlags flags;
    PyArena *arena;

    arena = PyArena_New();
    if (arena == NULL)
        return NULL;

    flags.cf_flags = 0;
    mod = PyParser_ASTFromStringObject(str, filename, start, &flags, arena);
    if (mod == NULL) {
        PyArena_Free(arena);
        return NULL;
    }
    st = PySymtable_BuildObject(mod, filename, 0);
    PyArena_Free(arena);
    return st;
}