resolve_json_schema_list_predicate(const char *predicate, const struct lys_node_list *list, int *parsed)
{
    const char *mod_name, *name;
    int mod_name_len, nam_len, has_predicate, i;
    struct lys_node *key;

    if (((i = parse_schema_json_predicate(predicate, &mod_name, &mod_name_len, &name, &nam_len, NULL, NULL, &has_predicate)) < 1)
            || !strncmp(name, ".", nam_len)) {
        LOGVAL(list->module->ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, predicate[-i], &predicate[-i]);
        return -1;
    }

    predicate += i;
    *parsed += i;

    if (!isdigit(name[0])) {
        for (i = 0; i < list->keys_size; ++i) {
            key = (struct lys_node *)list->keys[i];
            if (!strncmp(key->name, name, nam_len) && !key->name[nam_len]) {
                break;
            }
        }

        if (i == list->keys_size) {
            LOGVAL(list->module->ctx, LYE_PATH_INKEY, LY_VLOG_NONE, NULL, name);
            return -1;
        }
    }

    /* more predicates? */
    if (has_predicate) {
        return resolve_json_schema_list_predicate(predicate, list, parsed);
    }

    return 0;
}