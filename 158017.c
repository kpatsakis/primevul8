PyRun_FileFlags(FILE *fp, const char *p, int s, PyObject *g, PyObject *l,
                PyCompilerFlags *flags)
{
    return PyRun_FileExFlags(fp, p, s, g, l, 0, flags);
}