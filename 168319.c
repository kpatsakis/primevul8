sglob(char *pattern, int flags, int (*errfunc) (const char *, int),
      glob_t * pglob, unsigned long maxfiles, int maxdepth)
{
    simplify(pattern);
    return glob_(pattern, flags, errfunc, pglob, maxfiles, maxdepth);
}