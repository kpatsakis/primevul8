PyRun_SimpleFileEx(FILE *f, const char *p, int c)
{
    return PyRun_SimpleFileExFlags(f, p, c, NULL);
}