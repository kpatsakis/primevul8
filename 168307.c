glob2(Char *pathbuf, Char *pathbuf_last, Char *pathend, Char *pathend_last,
      Char *pattern, Char *pattern_last, glob_t *pglob, struct glob_lim *limitp,
      int recursion)
{
    struct stat sb;
    Char *p, *q;
    int anymeta;

    /*
     * Loop over pattern segments until end of pattern or until
     * segment with meta character found.
     */
    for (anymeta = 0;;) {
        if (*pattern == EOS) {      /* End of pattern? */
            *pathend = EOS;
            if (limitp->glim_stat++ >= pglob->gl_maxfiles) {
                errno = 0;
                *pathend++ = SEP;
                *pathend = EOS;
                return GLOB_NOSPACE;
            }
            if (g_lstat(pathbuf, &sb, pglob)) {
                return 0;
            }
            if (((pglob->gl_flags & GLOB_MARK) &&
                 pathend[-1] != SEP) &&
                (S_ISDIR(sb.st_mode) ||
                    (S_ISLNK(sb.st_mode) &&
                        (g_stat(pathbuf, &sb, pglob) == 0) &&
                        S_ISDIR(sb.st_mode)))) {
                if (pathend + 1 > pathend_last) {
                    return 1;
                }
                *pathend++ = SEP;
                *pathend = EOS;
            }
            ++pglob->gl_matchc;
            return globextend(pathbuf, pglob, limitp, &sb);
        }

        /* Find end of next segment, copy tentatively to pathend. */
        q = pathend;
        p = pattern;
        while (*p != EOS && *p != SEP) {
            if (ismeta(*p)) {
                anymeta = 1;
            }
            if (q + 1 > pathend_last) {
                return 1;
            }
            *q++ = *p++;
        }

        if (!anymeta) {     /* No expansion, do next segment. */
            pathend = q;
            pattern = p;
            while (*pattern == SEP) {
                if (pathend + 1 > pathend_last) {
                    return 1;
                }
                *pathend++ = *pattern++;
            }
        } else {
            /* Need expansion, recurse. */
            return glob3(pathbuf, pathbuf_last, pathend,
                         pathend_last, pattern, p, pattern_last,
                         pglob, limitp, recursion + 1);
        }
    }
    /* NOTREACHED */
}