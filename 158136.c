PyRun_AnyFile(FILE *fp, const char *name)
{
    return PyRun_AnyFileExFlags(fp, name, 0, NULL);
}