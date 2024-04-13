PyParser_ASTFromString(const char *s, const char *filename_str, int start,
                       PyCompilerFlags *flags, PyArena *arena)
{
    PyObject *filename;
    mod_ty mod;
    filename = PyUnicode_DecodeFSDefault(filename_str);
    if (filename == NULL)
        return NULL;
    mod = PyParser_ASTFromStringObject(s, filename, start, flags, arena);
    Py_DECREF(filename);
    return mod;
}