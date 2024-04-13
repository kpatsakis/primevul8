static int listfile(const PureFileInfo * const fi, const char *name)
{
    int rval = 0;
    struct stat st;
    struct tm *t;
    char suffix[2] = { 0, 0 };
    char m[PATH_MAX + 1U];
    const char *format;

    if (fi == NULL) {
        if (lstat(name, &st) < 0) {
            return 0;
        }
    } else {
        st.st_size = fi->size;
        st.st_mtime = fi->mtime;
        st.st_mode = fi->mode;
        st.st_nlink = fi->nlink;
        st.st_uid = fi->uid;
        st.st_gid = fi->gid;
        name = FI_NAME(fi);
    }
#if defined(WITH_VIRTUAL_CHROOT) && defined(S_IFLNK) && defined(S_IFDIR)
    if (S_ISLNK(st.st_mode) && name[0] == '.' &&
        name[1] == '.' && name[2] == 0) {
        st.st_mode &= ~S_IFLNK;
        st.st_mode |= S_IFDIR;
    }  /* Hack to please some Windows clients that dislike ../ -> ../ */
#endif
#if !defined(MINIMAL) && !defined(ALWAYS_SHOW_SYMLINKS_AS_SYMLINKS)
    if (
# ifndef ALWAYS_SHOW_RESOLVED_SYMLINKS
        broken_client_compat != 0 &&
# endif
        S_ISLNK(st.st_mode)) {
        struct stat sts;

        if (stat(name, &sts) == 0 && !S_ISLNK(sts.st_mode)) {
            st = sts;
        }
    } /* Show non-dangling symlinks as files/directories */
#endif
#ifdef DISPLAY_FILES_IN_UTC_TIME
    t = gmtime((time_t *) &st.st_mtime);
#else
    t = localtime((time_t *) &st.st_mtime);
#endif
    if (t == NULL) {
        logfile(LOG_ERR, "{gm,local}gtime() for [%s]", name);
        return 0;
    }
    if (opt_F) {
        if (S_ISLNK(st.st_mode))
            suffix[0] = '@';
        else if (S_ISDIR(st.st_mode)) {
            suffix[0] = '/';
            rval = 1;
        } else if (st.st_mode & 010101) {
            suffix[0] = '*';
        }
    }
    if (opt_l) {
        strncpy(m, " ---------", (sizeof m) - (size_t) 1U);
        m[(sizeof m) - (size_t) 1U] = 0;
        switch (st.st_mode & S_IFMT) {
        case S_IFREG:
            m[0] = '-';
            break;
        case S_IFLNK:
            m[0] = 'l';
            break;            /* readlink() here? */
        case S_IFDIR:
            m[0] = 'd';
            rval = 1;
            break;
        }
        if (m[0] != ' ') {
            char *alloca_nameline;
            const size_t sizeof_nameline = PATH_MAX + PATH_MAX + 128U;
            char timeline[6U];

            if (st.st_mode & 0400) {
                m[1] = 'r';
            }
            if (st.st_mode & 0200) {
                m[2] = 'w';
            }
            if (st.st_mode & 0100) {
                m[3] = (char) (st.st_mode & 04000 ? 's' : 'x');
            } else if (st.st_mode & 04000) {
                m[3] = 'S';
            }
            if (st.st_mode & 040) {
                m[4] = 'r';
            }
            if (st.st_mode & 020) {
                m[5] = 'w';
            }
            if (st.st_mode & 010) {
                m[6] = (char) (st.st_mode & 02000 ? 's' : 'x');
            } else if (st.st_mode & 02000) {
                m[6] = 'S';
            }
            if (st.st_mode & 04) {
                m[7] = 'r';
            }
            if (st.st_mode & 02) {
                m[8] = 'w';
            }
            if (st.st_mode & 01) {
                m[9] = (char) (st.st_mode & 01000 ? 't' : 'x');
            } else if (st.st_mode & 01000) {
                m[9] = 'T';
            }
            if (time(NULL) - st.st_mtime > 180 * 24 * 60 * 60) {
                if (SNCHECK(snprintf(timeline, sizeof timeline, "%5d",
                                     t->tm_year + 1900), sizeof timeline)) {
                    _EXIT(EXIT_FAILURE);
                }
            } else {
                if (SNCHECK(snprintf(timeline, sizeof timeline, "%02d:%02d",
                                     t->tm_hour, t->tm_min), sizeof timeline)) {
                    _EXIT(EXIT_FAILURE);
                }
            }
            if ((alloca_nameline = ALLOCA(sizeof_nameline)) == NULL) {
                return 0;
            }
            if (st.st_size < 10000000000U) {
                format = "%s %4u %s %s %10llu %s %2d %s %s";
            } else {
                format = "%s %4u %s %s %18llu %s %2d %s %s";
            }
            if (SNCHECK(snprintf(alloca_nameline, sizeof_nameline,
                                 format,
                                 m, (unsigned int) st.st_nlink,
                                 getname(st.st_uid),
                                 getgroup(st.st_gid),
                                 (unsigned long long) st.st_size,
                                 months[t->tm_mon],
                                 t->tm_mday, timeline, name),
                        sizeof_nameline)) {
                ALLOCA_FREE(alloca_nameline);
                _EXIT(EXIT_FAILURE);
            }
            if (S_ISLNK(st.st_mode)) {
                char *p = alloca_nameline + strlen(alloca_nameline);
                {
                    ssize_t sx;

                    if ((sx = readlink(name, m, sizeof m - 1U)) > 0) {
                        m[sx] = 0;
                    } else {
                        m[0] = m[1] = '.';
                        m[2] = 0;
                    }
                }
                suffix[0] = 0;
                if (opt_F && stat(name, &st) == 0) {
                    if (S_ISLNK(st.st_mode)) {
                        suffix[0] = '@';
                    } else if (S_ISDIR(st.st_mode)) {
                        suffix[0] = '/';
                    } else if (st.st_mode & 010101) {
                        suffix[0] = '*';
                    }
                }
                /* 2 * PATH_MAX + gap should be enough, but be paranoid... */
                if (SNCHECK
                    (snprintf(p, (sizeof_nameline) - strlen(alloca_nameline),
                              " -> %s", m),
                     (sizeof_nameline) - strlen(alloca_nameline))) {
                    ALLOCA_FREE(alloca_nameline);
                    _EXIT(EXIT_FAILURE);
                }
            }
            addfile(alloca_nameline, suffix);
            ALLOCA_FREE(alloca_nameline);
        }                    /* hide non-downloadable files */
    } else {
        if (S_ISREG(st.st_mode) ||
            S_ISDIR(st.st_mode) || S_ISLNK(st.st_mode)) {
            addfile(name, suffix);
        }
    }
    return rval;
}