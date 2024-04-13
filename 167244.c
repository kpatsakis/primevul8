const char *getgroup(const gid_t gid)
{
    struct groupid *p;
    struct group *pwd = NULL;

    for (p = group_head; p; p = p->next) {
        if (p->gid == gid) {
            return p->name;
        }
    }
# ifndef ALWAYS_RESOLVE_IDS
    if (chrooted == 0)
# endif
    {
        pwd = getgrgid(gid);
    }
    if ((p = malloc(sizeof *p)) == NULL) {
        die_mem();
    }
    p->gid = gid;
    if ((p->name = malloc((size_t) 11U)) == NULL) {
        die_mem();
    }
    if (pwd != NULL) {
        if (SNCHECK(snprintf(p->name, (size_t) 11U, "%-10.10s",
                             pwd->gr_name), (size_t) 11U)) {
            _EXIT(EXIT_FAILURE);
        }
    } else {
        if (SNCHECK(snprintf(p->name, (size_t) 11U, "%-10d", gid),
                    (size_t) 11U)) {
            _EXIT(EXIT_FAILURE);
        }
    }
    p->next = group_head;
    group_head = p;

    return p->name;
}