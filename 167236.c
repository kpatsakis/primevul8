const char *getname(const uid_t uid)
{
    struct userid *p;
    struct passwd *pwd = NULL;

    for (p = user_head; p; p = p->next) {
        if (p->uid == uid) {
            return p->name;
        }
    }
    if (
# ifndef ALWAYS_RESOLVE_IDS
        chrooted == 0 &&
# endif
        authresult.slow_tilde_expansion == 0) {
        pwd = getpwuid(uid);
    }
    if ((p = malloc(sizeof *p)) == NULL) {
        die_mem();
    }
    p->uid = uid;
    if ((p->name = malloc((size_t) 11U)) == NULL) {
        die_mem();
    }
    if (pwd != NULL) {
        if (SNCHECK(snprintf(p->name, (size_t) 11U,
                             "%-10.10s", pwd->pw_name), (size_t) 11U)) {
            _EXIT(EXIT_FAILURE);
        }
    } else {
        if (SNCHECK(snprintf(p->name, (size_t) 11U, "%-10d", uid),
                    (size_t) 11U)) {
            _EXIT(EXIT_FAILURE);
        }
    }
    p->next = user_head;
    user_head = p;

    return p->name;
}