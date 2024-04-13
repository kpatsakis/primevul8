parse_instance_identifier(const char *id, const char **model, int *mod_len, const char **name, int *nam_len,
                          int *has_predicate)
{
    int parsed = 0, ret;

    assert(id && model && mod_len && name && nam_len);

    if (has_predicate) {
        *has_predicate = 0;
    }

    if (id[0] != '/') {
        return -parsed;
    }

    ++parsed;
    ++id;

    if ((ret = parse_identifier(id)) < 1) {
        return ret;
    }

    *name = id;
    *nam_len = ret;

    parsed += ret;
    id += ret;

    if (id[0] == ':') {
        /* we have prefix */
        *model = *name;
        *mod_len = *nam_len;

        ++parsed;
        ++id;

        if ((ret = parse_identifier(id)) < 1) {
            return ret;
        }

        *name = id;
        *nam_len = ret;

        parsed += ret;
        id += ret;
    }

    if (id[0] == '[' && has_predicate) {
        *has_predicate = 1;
    }

    return parsed;
}