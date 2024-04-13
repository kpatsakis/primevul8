void dolist(char *arg, const int on_ctrl_conn)
{
    int c;
    void *tls_fd = NULL;

    matches = 0U;

    opt_a = opt_C = opt_d = opt_F = opt_R = opt_r = opt_t = opt_S = 0;
    opt_l = 1;
    if (force_ls_a != 0) {
        opt_a = 1;
    }
    if (arg != NULL) {
        while (isspace((unsigned char) *arg)) {
            arg++;
        }
        while (*arg == '-') {
            while (arg++ && isalnum((unsigned char) *arg)) {
                switch (*arg) {
                case 'a':
                    opt_a = 1;
                    break;
                case 'l':
                    opt_l = 1;
                    opt_C = 0;
                    break;
                case '1':
                    opt_l = opt_C = 0;
                    break;
                case 'C':
                    opt_l = 0;
                    opt_C = 1;
                    break;
                case 'F':
                    opt_F = 1;
                    break;
                case 'R':
                    opt_R = 1;
                    break;
                case 'd':
                    opt_d = 1;
                    break;
                case 'r':
                    opt_r = 1;
                    break;
                case 't':
                    opt_t = 1;
                    opt_S = 0;
                    break;
                case 'S':
                    opt_S = 1;
                    opt_t = 0;
                    break;
                }
            }
            while (isspace((unsigned char) *arg)) {
                arg++;
            }
        }
    }
    if (on_ctrl_conn == 0) {
        opendata();
        if ((c = xferfd) == -1) {
            return;
        }
        doreply();
#ifdef WITH_TLS
        if (data_protection_level == CPL_PRIVATE) {
            tls_init_data_session(xferfd, passive);
            tls_fd = tls_data_cnx;
        }
#endif
    } else {                           /* STAT command */
        c = clientfd;
#ifdef WITH_TLS
        if (tls_cnx != NULL) {
            secure_safe_write(tls_cnx, "213-STAT" CRLF,
                              sizeof "213-STAT" CRLF - 1U);
            tls_fd = tls_cnx;
        }
        else
#endif
        {
            safe_write(c, "213-STAT" CRLF, sizeof "213-STAT" CRLF - 1U, -1);
        }
    }
    if (arg != NULL && *arg != 0) {
        int justone;

        justone = 1;            /* just one argument, so don't print dir name */

        do {
            glob_t g;
            int a;
            char *endarg;

            if ((endarg = unescape_and_return_next_file(arg)) != NULL) {
                justone = 0;
            }

            /* Expand ~ here if needed */

            alarm(GLOB_TIMEOUT);
            memset(&g, 0, sizeof g);
            a = sglob(arg,
                      opt_a ? (GLOB_PERIOD | GLOB_LIMIT) : GLOB_LIMIT,
                      NULL, &g, max_ls_files + 2, max_ls_depth * 2);
            alarm(0);
            if (a == 0) {
                char **path;

                if (g.gl_pathc <= 0) {
                    path = NULL;
                } else {
                    path = g.gl_pathv;
                }
                if (path != NULL && path[0] != NULL && path[1] != NULL) {
                    justone = 0;
                }
                while (path != NULL && *path != NULL) {
                    struct stat st;

                    if (stat(*path, &st) == 0) {
                        if (opt_d || !(S_ISDIR(st.st_mode))) {
                            listfile(NULL, *path);
                            **path = 0;
                        }
                    } else {
                        **path = 0;
                    }
                    path++;
                }
                outputfiles(c, tls_fd);    /* in case of opt_C */
                path = g.gl_pathv;
                while (path != NULL && *path != NULL) {
                    if (matches >= max_ls_files) {
                        break;
                    }
                    if (**path != 0) {
                        if (!justone) {
                            wrstr(c, tls_fd, "\r\n\r\n");
                            wrstr(c, tls_fd, *path);
                            wrstr(c, tls_fd, ":\r\n\r\n");
                        }
                        if (!chdir(*path)) {
                            listdir(0U, c, tls_fd, *path);
                            if (chdir(wd)) {
                                die(421, LOG_ERR, "chdir: %s", strerror(errno));
                            }
                        }
                    }
                    path++;
                }
            } else {
                if (a == GLOB_NOSPACE) {
                    addreply(226, MSG_GLOB_NO_MEMORY, arg);
                    addreply_noformat(0, MSG_PROBABLY_DENIED);
                } else if (a == GLOB_ABEND) {
                    addreply(226, MSG_GLOB_READ_ERROR, arg);
                } else if (a != GLOB_NOMATCH) {
                    addreply(226, MSG_GLOB_READ_ERROR, arg);
                    addreply_noformat(0, MSG_PROBABLY_DENIED);
                }
            }
            globfree(&g);
            arg = endarg;
        } while (arg != NULL);
    } else {
        if (opt_d) {
            listfile(NULL, ".");
        } else {
            listdir(0U, c, tls_fd, ".");
        }
        outputfiles(c, tls_fd);
    }
    wrstr(c, tls_fd, NULL);
    if (on_ctrl_conn == 0) {
#ifdef WITH_TLS
        closedata();
#endif
        close(c);
    } else {
        addreply_noformat(213, "End.");
        goto end;
    }
    if (opt_a || opt_C || opt_d || opt_F || opt_l || opt_r || opt_R ||
        opt_t || opt_S)
        addreply(0, "Options: %s%s%s%s%s%s%s%s%s",
                 opt_a ? "-a " : "",
                 opt_C ? "-C " : "",
                 opt_d ? "-d " : "",
                 opt_F ? "-F " : "",
                 opt_l ? "-l " : "",
                 opt_r ? "-r " : "",
                 opt_R ? "-R " : "", opt_S ? "-S " : "",
                 opt_t ? "-t" : "");
    if (matches >= max_ls_files) {
        addreply(226, MSG_LS_TRUNCATED, matches);
    } else {
        addreply(226, MSG_LS_SUCCESS, matches);
    }
end:
    if (chdir(wd)) {
        die(421, LOG_ERR, "chdir: %s", strerror(errno));
    }
}