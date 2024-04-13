PyRun_InteractiveOneFlags(FILE *fp, const char *filename_str, PyCompilerFlags *flags)
{
    PyObject *filename;
    int res;

    filename = PyUnicode_DecodeFSDefault(filename_str);
    if (filename == NULL) {
        PyErr_Print();
        return -1;
    }
    res = PyRun_InteractiveOneObject(fp, filename, flags);
    Py_DECREF(filename);
    return res;
}