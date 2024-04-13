string_object_to_py_ast(const char *str, PyObject *filename, int start,
                       PyCompilerFlags *flags, int feature_version)
{
    mod_ty mod;
    PyObject *result;
    PyArena *arena = PyArena_New();
    if (arena == NULL)
        return NULL;

    mod = string_object_to_c_ast(str, filename, start, flags, feature_version, arena);
    if (mod == NULL) {
        PyArena_Free(arena);
        return NULL;
    }

    result = Ta3AST_mod2obj(mod);
    PyArena_Free(arena);
    return result;
}