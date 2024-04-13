resolve_extended_schema_nodeid_predicate(const char *nodeid, const struct lys_node *node,
                                         const struct lys_module *cur_module, int *nodeid_end)
{
    int mod_len, nam_len, has_predicate, r, i;
    const char *model, *name;
    struct lys_node_list *list;

    if (!(node->nodetype & (LYS_LIST | LYS_LEAFLIST))) {
        return 1;
    }

    list = (struct lys_node_list *)node;
    do {
        r = parse_schema_json_predicate(nodeid, &model, &mod_len, &name, &nam_len, NULL, NULL, &has_predicate);
        if (r < 1) {
            LOGVAL(cur_module->ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, nodeid[r], &nodeid[r]);
            return -1;
        }
        nodeid += r;

        if (node->nodetype == LYS_LEAFLIST) {
            /* just check syntax */
            if (model || !name || (name[0] != '.') || has_predicate) {
                return 1;
            }
            break;
        } else {
            /* check the key */
            for (i = 0; i < list->keys_size; ++i) {
                if (strncmp(list->keys[i]->name, name, nam_len) || list->keys[i]->name[nam_len]) {
                    continue;
                }
                if (model) {
                    if (strncmp(lys_node_module((struct lys_node *)list->keys[i])->name, model, mod_len)
                            || lys_node_module((struct lys_node *)list->keys[i])->name[mod_len]) {
                        continue;
                    }
                } else {
                    if (lys_node_module((struct lys_node *)list->keys[i]) != cur_module) {
                        continue;
                    }
                }

                /* match */
                break;
            }

            if (i == list->keys_size) {
                return 1;
            }
        }
    } while (has_predicate);

    if (!nodeid[0]) {
        *nodeid_end = 1;
    }
    return 0;
}