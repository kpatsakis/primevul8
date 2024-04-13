parse_schema_nodeid(const char *id, const char **mod_name, int *mod_name_len, const char **name, int *nam_len,
                    int *is_relative, int *has_predicate, int *all_desc, int extended)
{
    int parsed = 0, ret;

    assert(id);
    assert(is_relative);

    if (has_predicate) {
        *has_predicate = 0;
    }

    if (id[0] != '/') {
        if (*is_relative != -1) {
            return -parsed;
        } else {
            *is_relative = 1;
        }
        if (!strncmp(id, "./", 2)) {
            parsed += 2;
            id += 2;
        }
    } else {
        if (*is_relative == -1) {
            *is_relative = 0;
        }
        ++parsed;
        ++id;
    }

    if ((ret = parse_node_identifier(id, mod_name, mod_name_len, name, nam_len, all_desc, extended)) < 1) {
        return -parsed + ret;
    }

    parsed += ret;
    id += ret;

    if ((id[0] == '[') && has_predicate) {
        *has_predicate = 1;
    }

    return parsed;
}