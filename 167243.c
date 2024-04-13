void domlsd(const char *base)
{
    char           line[PATH_MAX + 1];
    DIR           *dir = NULL;
    void          *tls_fd = NULL;
    struct dirent *de;
    unsigned int   matches = 0;
    int            c;

    if (*base != 0 && chdir(base) != 0) {
        if (*base++ == '-') {
            while (!isspace((unsigned char) *base++));
            while (isspace((unsigned char) *base++));
            if (*base != 0 && chdir(base) != 0) {
                addreply_noformat(550, MSG_STAT_FAILURE2);
                return;
            }
        } else {
            addreply_noformat(550, MSG_STAT_FAILURE2);
            return;
        }
    }
    if ((dir = opendir(".")) == NULL) {
        addreply_noformat(550, MSG_STAT_FAILURE2);
        goto bye;
    }
    opendata();
    if ((c = xferfd) == -1) {
        goto bye;
    }
    doreply();
#ifdef WITH_TLS
    if (data_protection_level == CPL_PRIVATE) {
        tls_init_data_session(xferfd, passive);
        tls_fd = tls_data_cnx;
    }
#endif
    while ((de = readdir(dir)) != NULL) {
        if (checkprintable(de->d_name) != 0 ||
            modernformat(de->d_name, line, sizeof line, "") < 0) {
            continue;
        }
        wrstr(c, tls_fd, line);
        wrstr(c, tls_fd, "\r\n");
        matches++;
        if (matches >= max_ls_files) {
            break;
        }
    }
    wrstr(c, tls_fd, NULL);
    closedata();
    if (matches >= max_ls_files) {
        addreply(226, MSG_LS_TRUNCATED, matches);
    } else {
        addreply(226, MSG_LS_SUCCESS, matches);
    }
bye:
    if (dir != NULL) {
        closedir(dir);
    }
    if (chdir(wd) != 0) {
        die(421, LOG_ERR, "chdir: %s", strerror(errno));
    }
}