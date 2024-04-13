Py_SymtableString(const char *str, const char *filename_str, int start)
{
    PyObject *filename;
    struct symtable *st;

    filename = PyUnicode_DecodeFSDefault(filename_str);
    if (filename == NULL)
        return NULL;
    st = Py_SymtableStringObject(str, filename, start);
    Py_DECREF(filename);
    return st;
}