parse_node_identifier(const char *id, const char **mod_name, int *mod_name_len, const char **name, int *nam_len,
                      int *all_desc, int extended)
{
    int parsed = 0, ret, all_desc_local = 0, first_id_len;
    const char *first_id;

    assert(id);
    assert((mod_name && mod_name_len) || (!mod_name && !mod_name_len));
    assert((name && nam_len) || (!name && !nam_len));

    if (mod_name) {
        *mod_name = NULL;
        *mod_name_len = 0;
    }
    if (name) {
        *name = NULL;
        *nam_len = 0;
    }

    if (extended) {
        /* try to parse only the extended expressions */
        if (id[parsed] == '/') {
            if (all_desc) {
                *all_desc = 1;
            }
            all_desc_local = 1;
        } else {
            if (all_desc) {
                *all_desc = 0;
            }
        }

        /* is there a prefix? */
        ret = parse_identifier(id + all_desc_local);
        if (ret > 0) {
            if (id[all_desc_local + ret] != ':') {
                /* this is not a prefix, so not an extended id */
                goto standard_id;
            }

            if (mod_name) {
                *mod_name = id + all_desc_local;
                *mod_name_len = ret;
            }

            /* "/" and ":" */
            ret += all_desc_local + 1;
        } else {
            ret = all_desc_local;
        }

        /* parse either "*" or "." */
        if (*(id + ret) == '*') {
            if (name) {
                *name = id + ret;
                *nam_len = 1;
            }
            ++ret;

            return ret;
        } else if (*(id + ret) == '.') {
            if (!all_desc_local) {
                /* /. is redundant expression, we do not accept it */
                return -ret;
            }

            if (name) {
                *name = id + ret;
                *nam_len = 1;
            }
            ++ret;

            return ret;
        } else if (*(id + ret) == '#') {
            if (all_desc_local || !ret) {
                /* no prefix */
                return 0;
            }
            parsed = ret + 1;
            if ((ret = parse_identifier(id + parsed)) < 1) {
                return -parsed + ret;
            }
            *name = id + parsed - 1;
            *nam_len = ret + 1;
            return parsed + ret;
        }
        /* else a standard id, parse it all again */
    }

standard_id:
    if ((ret = parse_identifier(id)) < 1) {
        return ret;
    }

    first_id = id;
    first_id_len = ret;

    parsed += ret;
    id += ret;

    /* there is prefix */
    if (id[0] == ':') {
        ++parsed;
        ++id;

    /* there isn't */
    } else {
        if (name) {
            *name = first_id;
            *nam_len = first_id_len;
        }

        return parsed;
    }

    /* identifier (node name) */
    if ((ret = parse_identifier(id)) < 1) {
        return -parsed + ret;
    }

    if (mod_name) {
        *mod_name = first_id;
        *mod_name_len = first_id_len;
    }
    if (name) {
        *name = id;
        *nam_len = ret;
    }

    return parsed + ret;
}