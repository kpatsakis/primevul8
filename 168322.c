glob1(Char *pattern, Char *pattern_last, glob_t *pglob, struct glob_lim *limitp,
      int recursion)
{
    Char pathbuf[PATH_MAX];

    /* A null pathname is invalid -- POSIX 1003.1 sect. 2.4. */
    if (*pattern == EOS) {
        return 0;
    }
    return glob2(pathbuf, pathbuf + PATH_MAX - 1,
                 pathbuf, pathbuf + PATH_MAX - 1,
                 pattern, pattern_last, pglob, limitp, recursion);
}