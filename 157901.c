PyRun_InteractiveOne(FILE *f, const char *p)
{
    return PyRun_InteractiveOneFlags(f, p, NULL);
}