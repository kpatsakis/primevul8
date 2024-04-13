PyRun_InteractiveOneObject(FILE *fp, PyObject *filename, PyCompilerFlags *flags)
{
    int res;

    res = PyRun_InteractiveOneObjectEx(fp, filename, flags);
    if (res == -1) {
        PyErr_Print();
        flush_io();
    }
    return res;
}