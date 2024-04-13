glob(const char *pattern, int flags, int (*errfunc) (const char *, int),
     glob_t * pglob)
{
    return glob_(pattern, flags, errfunc, pglob, (unsigned long) -1, 0);
}