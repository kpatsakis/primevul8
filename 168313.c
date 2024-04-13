globexp1(const Char *pattern, glob_t *pglob, struct glob_lim *limitp,
         int recursion)
{
    const Char* ptr = pattern;

    if (pglob->gl_maxdepth > 0 && recursion > pglob->gl_maxdepth) {
        errno = 0;
        return 0;
    }
    /* Protect a single {}, for find(1), like csh */
    if (pattern[0] == LBRACE && pattern[1] == RBRACE && pattern[2] == EOS) {
        return glob0(pattern, pglob, limitp);
    }
    if ((ptr = (const Char *) g_strchr(ptr, LBRACE)) != NULL) {
        return globexp2(ptr, pattern, pglob, limitp, recursion + 1);
    }
    return glob0(pattern, pglob, limitp);
}