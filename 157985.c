PyAST_FromNode(const node *n, PyCompilerFlags *flags, const char *filename_str,
               PyArena *arena)
{
    mod_ty mod;
    PyObject *filename;
    filename = PyUnicode_DecodeFSDefault(filename_str);
    if (filename == NULL)
        return NULL;
    mod = PyAST_FromNodeObject(n, flags, filename, arena);
    Py_DECREF(filename);
    return mod;

}