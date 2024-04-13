parse_predicate(const char *id, const char **model, int *mod_len, const char **name, int *nam_len,
                const char **value, int *val_len, int *has_predicate)
{
    const char *ptr;
    int parsed = 0, ret;
    char quote;

    assert(id);
    if (model) {
        assert(mod_len);
        *model = NULL;
        *mod_len = 0;
    }
    if (name) {
        assert(nam_len);
        *name = NULL;
        *nam_len = 0;
    }
    if (value) {
        assert(val_len);
        *value = NULL;
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

    /* pos */
    if (isdigit(id[0])) {
        if (name) {
            *name = id;
        }

        if (id[0] == '0') {
            return -parsed;
        }

        while (isdigit(id[0])) {
            ++parsed;
            ++id;
        }

        if (nam_len) {
            *nam_len = id-(*name);
        }

    /* "." or node-identifier */
    } else {
        if (id[0] == '.') {
            if (name) {
                *name = id;
            }
            if (nam_len) {
                *nam_len = 1;
            }

            ++parsed;
            ++id;

        } else {
            if ((ret = parse_node_identifier(id, model, mod_len, name, nam_len, NULL, 0)) < 1) {
                return -parsed + ret;
            }

            parsed += ret;
            id += ret;
        }

        while (isspace(id[0])) {
            ++parsed;
            ++id;
        }

        if (id[0] != '=') {
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
    }

    while (isspace(id[0])) {
        ++parsed;
        ++id;
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