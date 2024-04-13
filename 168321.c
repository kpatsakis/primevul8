g_stat(Char *fn, struct stat *sb, glob_t *pglob)
{
    char buf[PATH_MAX];

    if (g_Ctoc(fn, buf, sizeof(buf))) {
        return -1;
    }
    if (pglob->gl_flags & GLOB_ALTDIRFUNC) {
        return (*pglob->gl_stat)(buf, sb);
    }
    return stat(buf, sb);
}