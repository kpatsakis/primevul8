glob3(Char *pathbuf, Char *pathbuf_last, Char *pathend, Char *pathend_last,
      Char *pattern, Char *restpattern, Char *restpattern_last, glob_t *pglob,
      struct glob_lim *limitp, int recursion)
{
    struct dirent *dp;
    DIR *dirp;
    int err;
    char buf[PATH_MAX];

    /*
     * The readdirfunc declaration can't be prototyped, because it is
     * assigned, below, to two functions which are prototyped in glob.h
     * and dirent.h as taking pointers to differently typed opaque
     * structures.
     */
    struct dirent *(*readdirfunc)(void *);

    if (pathend > pathend_last) {
        return 1;
    }
    *pathend = EOS;
    errno = 0;

    if (recursion >= pglob->gl_maxdepth) {
        return GLOB_NOSPACE;
    }
    if ((dirp = g_opendir(pathbuf, pglob)) == NULL) {
        /* TODO: don't call for ENOENT or ENOTDIR? */
        if (pglob->gl_errfunc) {
            if (g_Ctoc(pathbuf, buf, sizeof(buf))) {
                return GLOB_ABORTED;
            }
            if (pglob->gl_errfunc(buf, errno) ||
                pglob->gl_flags & GLOB_ERR) {
                return GLOB_ABORTED;
            }
        }
        return 0;
    }

    err = 0;

    /* Search directory for matching names. */
    if (pglob->gl_flags & GLOB_ALTDIRFUNC) {
        readdirfunc = pglob->gl_readdir;
    } else {
        readdirfunc = (struct dirent *(*)(void *))readdir;
    }
    while ((dp = (*readdirfunc)(dirp))) {
        unsigned char *sc;
        Char *dc;

        if (limitp->glim_readdir++ >= pglob->gl_maxfiles) {
            errno = 0;
            *pathend++ = SEP;
            *pathend = EOS;
            err = GLOB_NOSPACE;
            break;
        }

        /* Initial DOT must be matched literally. */
        if (dp->d_name[0] == DOT && *pattern != DOT) {
            continue;
        }
        dc = pathend;
        sc = (unsigned char *) dp->d_name;
        while (dc < pathend_last && (*dc++ = *sc++) != EOS)
            ;
        if (dc >= pathend_last) {
            *dc = EOS;
            err = 1;
            break;
        }

        if (!match(pathend, pattern, restpattern, pglob->gl_maxdepth)) {
            *pathend = EOS;
            continue;
        }
        err = glob2(pathbuf, pathbuf_last, --dc, pathend_last,
                    restpattern, restpattern_last, pglob, limitp, recursion);
        if (err) {
            break;
        }
    }

    if (pglob->gl_flags & GLOB_ALTDIRFUNC) {
        (*pglob->gl_closedir)(dirp);
    } else {
        closedir(dirp);
    }
    return err;
}