Py_CompileStringExFlags(const char *str, const char *filename_str, int start,
                        PyCompilerFlags *flags, int optimize)
{
    PyObject *filename, *co;
    filename = PyUnicode_DecodeFSDefault(filename_str);
    if (filename == NULL)
        return NULL;
    co = Py_CompileStringObject(str, filename, start, flags, optimize);
    Py_DECREF(filename);
    return co;
}