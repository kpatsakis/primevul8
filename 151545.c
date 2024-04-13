parse_path_arg(const struct lys_module *mod, const char *id, const char **prefix, int *pref_len,
               const char **name, int *nam_len, int *parent_times, int *has_predicate)
{
    int parsed = 0, ret, par_times = 0;

    assert(id);
    assert(parent_times);
    if (prefix) {
        *prefix = NULL;
    }
    if (pref_len) {
        *pref_len = 0;
    }
    if (name) {
        *name = NULL;
    }
    if (nam_len) {
        *nam_len = 0;
    }
    if (has_predicate) {
        *has_predicate = 0;
    }

    if (!*parent_times && !strncmp(id, "..", 2)) {
        ++par_times;

        parsed += 2;
        id += 2;

        while (!strncmp(id, "/..", 3)) {
            ++par_times;

            parsed += 3;
            id += 3;
        }
    }

    if (!*parent_times) {
        if (par_times) {
            *parent_times = par_times;
        } else {
            *parent_times = -1;
        }
    }

    if (id[0] != '/') {
        return -parsed;
    }

    /* skip '/' */
    ++parsed;
    ++id;

    /* node-identifier ([prefix:]identifier) */
    if ((ret = parse_node_identifier(id, prefix, pref_len, name, nam_len, NULL, 0)) < 1) {
        return -parsed - ret;
    }
    if (prefix && !(*prefix)) {
        /* actually we always need prefix even it is not specified */
        *prefix = lys_main_module(mod)->name;
        *pref_len = strlen(*prefix);
    }

    parsed += ret;
    id += ret;

    /* there is no predicate */
    if ((id[0] == '/') || !id[0]) {
        return parsed;
    } else if (id[0] != '[') {
        return -parsed;
    }

    if (has_predicate) {
        *has_predicate = 1;
    }

    return parsed;
}