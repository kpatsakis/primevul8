PyRun_AnyFileEx(FILE *fp, const char *name, int closeit)
{
    return PyRun_AnyFileExFlags(fp, name, closeit, NULL);
}