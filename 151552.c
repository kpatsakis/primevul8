parse_schema_json_predicate(const char *id, const char **mod_name, int *mod_name_len, const char **name, int *nam_len,
                            const char **value, int *val_len, int *has_predicate)
{
    const char *ptr;
    int parsed = 0, ret;
    char quote;

    assert(id);
    if (mod_name) {
        *mod_name = NULL;
    }
    if (mod_name_len) {
        *mod_name_len = 0;
    }
    if (name) {
        *name = NULL;
    }
    if (nam_len) {
        *nam_len = 0;
    }
    if (value) {
        *value = NULL;
    }
    if (val_len) {
        *val_len = 0;
    }
    if (has_predicate) {
        *has_predicate = 0;
    }

    if (id[0] != '[') {
        return -parsed;
    }

    ++parsed;
    ++id;

    while (isspace(id[0])) {
        ++parsed;
        ++id;
    }

    /* identifier */
    if (id[0] == '.') {
        ret = 1;

        if (name) {
            *name = id;
        }
        if (nam_len) {
            *nam_len = ret;
        }
    } else if (isdigit(id[0])) {
        if (id[0] == '0') {
            return -parsed;
        }
        ret = 1;
        while (isdigit(id[ret])) {
            ++ret;
        }

        if (name) {
            *name = id;
        }
        if (nam_len) {
            *nam_len = ret;
        }
    } else if ((ret = parse_node_identifier(id, mod_name, mod_name_len, name, nam_len, NULL, 0)) < 1) {
        return -parsed + ret;
    }

    parsed += ret;
    id += ret;

    while (isspace(id[0])) {
        ++parsed;
        ++id;
    }

    /* there is value as well */
    if (id[0] == '=') {
        if (name && isdigit(**name)) {
            return -parsed;
        }

        ++parsed;
        ++id;

        while (isspace(id[0])) {
            ++parsed;
            ++id;
        }

        /* ((DQUOTE string DQUOTE) / (SQUOTE string SQUOTE)) */
        if ((id[0] == '\"') || (id[0] == '\'')) {
            quote = id[0];

            ++parsed;
            ++id;

            if ((ptr = strchr(id, quote)) == NULL) {
                return -parsed;
            }
            ret = ptr - id;

            if (value) {
                *value = id;
            }
            if (val_len) {
                *val_len = ret;
            }

            parsed += ret + 1;
            id += ret + 1;
        } else {
            return -parsed;
        }

        while (isspace(id[0])) {
            ++parsed;
            ++id;
        }
    }

    if (id[0] != ']') {
        return -parsed;
    }

    ++parsed;
    ++id;

    if ((id[0] == '[') && has_predicate) {
        *has_predicate = 1;
    }

    return parsed;
}