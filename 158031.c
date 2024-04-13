Py_CompileStringObject(const char *str, PyObject *filename, int start,
                       PyCompilerFlags *flags, int optimize)
{
    PyCodeObject *co;
    mod_ty mod;
    PyArena *arena = PyArena_New();
    if (arena == NULL)
        return NULL;

    mod = PyParser_ASTFromStringObject(str, filename, start, flags, arena);
    if (mod == NULL) {
        PyArena_Free(arena);
        return NULL;
    }
    if (flags && (flags->cf_flags & PyCF_ONLY_AST)) {
        PyObject *result = PyAST_mod2obj(mod);
        PyArena_Free(arena);
        return result;
    }
    co = PyAST_CompileObject(mod, filename, flags, optimize, arena);
    PyArena_Free(arena);
    return (PyObject *)co;
}