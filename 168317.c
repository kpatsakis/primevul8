g_opendir(Char *str, glob_t *pglob)
{
    char buf[PATH_MAX];

    if (!*str) {
        buf[0] = '.';
        buf[1] = 0;
    } else {
        if (g_Ctoc(str, buf, sizeof(buf))) {
            return NULL;
        }
    }

    if (pglob->gl_flags & GLOB_ALTDIRFUNC) {
        return (*pglob->gl_opendir)(buf);
    }

    return opendir(buf);
}