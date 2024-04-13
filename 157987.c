PyRun_SimpleFile(FILE *f, const char *p)
{
    return PyRun_SimpleFileExFlags(f, p, 0, NULL);
}