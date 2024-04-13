globfree(glob_t *pglob)
{
    int i;
    char **pp;

    if (pglob->gl_pathv != NULL) {
        pp = pglob->gl_pathv + pglob->gl_offs;
        for (i = pglob->gl_pathc; i--; ++pp) {
            if (*pp) {
                free(*pp);
            }
        }
        free(pglob->gl_pathv);
        pglob->gl_pathv = NULL;
    }
    if (pglob->gl_statv != NULL) {
        for (i = 0; i < pglob->gl_pathc; i++) {
            if (pglob->gl_statv[i] != NULL) {
                free(pglob->gl_statv[i]);
            }
        }
        free(pglob->gl_statv);
        pglob->gl_statv = NULL;
    }
}