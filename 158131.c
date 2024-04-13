PyRun_InteractiveLoop(FILE *f, const char *p)
{
    return PyRun_InteractiveLoopFlags(f, p, NULL);
}