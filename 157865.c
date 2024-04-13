PyRun_AnyFileFlags(FILE *fp, const char *name, PyCompilerFlags *flags)
{
    return PyRun_AnyFileExFlags(fp, name, 0, flags);
}