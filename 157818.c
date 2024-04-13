PyParser_ASTFromFile(FILE *fp, const char *filename_str, const char* enc,
                     int start, const char *ps1,
                     const char *ps2, PyCompilerFlags *flags, int *errcode,
                     PyArena *arena)
{
    mod_ty mod;
    PyObject *filename;
    filename = PyUnicode_DecodeFSDefault(filename_str);
    if (filename == NULL)
        return NULL;
    mod = PyParser_ASTFromFileObject(fp, filename, enc, start, ps1, ps2,
                                     flags, errcode, arena);
    Py_DECREF(filename);
    return mod;
}