void donlst(const char *base)
{
    char           line[PATH_MAX + 3U];
    DIR           *dir;
    void          *tls_fd = NULL;
    struct dirent *de;
    size_t         name_len;
    unsigned int   matches = 0;
    int            c;
    int            base_has_trailing_slash = 0;

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
    if (*base !=0 && base[strlen(base) - 1U] == '/') {
        base_has_trailing_slash = 1;
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
        if (checkprintable(de->d_name) != 0) {
            continue;
        }
        name_len = strlen(de->d_name);
        if (name_len > sizeof line - 3U) {
            continue;
        }
        memcpy(line, de->d_name, name_len);
        line[name_len] = '\r';
        line[name_len + 1] = '\n';
        line[name_len + 2] = 0;
        if (*base) {
            wrstr(c, tls_fd, base);
            if (base_has_trailing_slash == 0) {
                wrstr(c, tls_fd, "/");
            }
        }
        wrstr(c, tls_fd, line);
        matches++;
        if (matches >= max_ls_files) {
            break;
        }
    }
    closedir(dir);
    wrstr(c, tls_fd, NULL);
    closedata();
    if (matches >= max_ls_files) {
        addreply(226, MSG_LS_TRUNCATED, matches);
    } else {
        addreply(226, MSG_LS_SUCCESS, matches);
    }
bye:
    if (chdir(wd) != 0) {
        die(421, LOG_ERR, "chdir: %s", strerror(errno));
    }
}