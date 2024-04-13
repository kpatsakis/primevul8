globextend(const Char *path, glob_t *pglob, struct glob_lim *limitp,
           struct stat *sb)
{
    char **pathv;
    ssize_t i;
    size_t newn, len;
    char *copy = NULL;
    const Char *p;
    struct stat **statv;

    newn = 2 + pglob->gl_pathc + pglob->gl_offs;
    if (pglob->gl_offs >= INT_MAX ||
        pglob->gl_pathc >= INT_MAX ||
        newn >= INT_MAX ||
        SIZE_MAX / sizeof(*pathv) <= newn ||
        SIZE_MAX / sizeof(*statv) <= newn) {
nospace:
        for (i = pglob->gl_offs; i < (ssize_t)(newn - 2); i++) {
            if (pglob->gl_pathv && pglob->gl_pathv[i]) {
                free(pglob->gl_pathv[i]);
            }
            if ((pglob->gl_flags & GLOB_KEEPSTAT) != 0 &&
                pglob->gl_pathv && pglob->gl_pathv[i]) {
                free(pglob->gl_statv[i]);
            }
        }
        if (pglob->gl_pathv) {
            free(pglob->gl_pathv);
            pglob->gl_pathv = NULL;
        }
        if (pglob->gl_statv) {
            free(pglob->gl_statv);
            pglob->gl_statv = NULL;
        }
        return GLOB_NOSPACE;
    }

    pathv = realloc(pglob->gl_pathv, newn * sizeof(*pathv));
    if (pathv == NULL) {
        goto nospace;
    }
    if (pglob->gl_pathv == NULL && pglob->gl_offs > 0) {
        /* first time around -- clear initial gl_offs items */
        pathv += pglob->gl_offs;
        for (i = pglob->gl_offs; --i >= 0; ) {
            *--pathv = NULL;
        }
    }
    pglob->gl_pathv = pathv;

    if ((pglob->gl_flags & GLOB_KEEPSTAT) != 0) {
        statv = realloc(pglob->gl_statv, newn * sizeof(*statv));
        if (statv == NULL) {
            goto nospace;
        }
        if (pglob->gl_statv == NULL && pglob->gl_offs > 0) {
            /* first time around -- clear initial gl_offs items */
            statv += pglob->gl_offs;
            for (i = pglob->gl_offs; --i >= 0; ) {
                *--statv = NULL;
            }
        }
        pglob->gl_statv = statv;
        if (sb == NULL) {
            statv[pglob->gl_offs + pglob->gl_pathc] = NULL;
        } else {
            limitp->glim_malloc += sizeof(**statv);
            if (limitp->glim_malloc >= GLOB_LIMIT_MALLOC) {
                errno = 0;
                return GLOB_NOSPACE;
            }
            if ((statv[pglob->gl_offs + pglob->gl_pathc] =
                 malloc(sizeof(**statv))) == NULL) {
                goto copy_error;
            }
            memcpy(statv[pglob->gl_offs + pglob->gl_pathc], sb,
                   sizeof(*sb));
        }
        statv[pglob->gl_offs + pglob->gl_pathc + 1] = NULL;
    }

    for (p = path; *p++;)
        ;
    len = (size_t)(p - path);
    limitp->glim_malloc += len;
    if ((copy = malloc(len)) != NULL) {
        if (g_Ctoc(path, copy, len)) {
            free(copy);
            return GLOB_NOSPACE;
        }
        pathv[pglob->gl_offs + pglob->gl_pathc++] = copy;
    }
    pathv[pglob->gl_offs + pglob->gl_pathc] = NULL;

    if ((newn * sizeof(*pathv)) + limitp->glim_malloc > GLOB_LIMIT_MALLOC) {
        errno = 0;
        return GLOB_NOSPACE;
    }
copy_error:
    return copy == NULL ? GLOB_NOSPACE : 0;
}