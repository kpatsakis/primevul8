resolve_hash_table_find_equal(void *val1_p, void *val2_p, int mod, void *UNUSED(cb_data))
{
    struct lyd_node *val2, *elem2;
    struct parsed_pred pp;
    const char *str;
    int i;

    assert(!mod);
    (void)mod;

    pp = *((struct parsed_pred *)val1_p);
    val2 = *((struct lyd_node **)val2_p);

    if (val2->schema != pp.schema) {
        return 0;
    }

    switch (val2->schema->nodetype) {
    case LYS_CONTAINER:
    case LYS_LEAF:
    case LYS_ANYXML:
    case LYS_ANYDATA:
        return 1;
    case LYS_LEAFLIST:
        str = ((struct lyd_node_leaf_list *)val2)->value_str;
        if (!strncmp(str, pp.pred[0].value, pp.pred[0].val_len) && !str[pp.pred[0].val_len]) {
            return 1;
        }
        return 0;
    case LYS_LIST:
        assert(((struct lys_node_list *)val2->schema)->keys_size);
        assert(((struct lys_node_list *)val2->schema)->keys_size == pp.len);

        /* lists with keys, their equivalence is based on their keys */
        elem2 = val2->child;
        /* the exact data order is guaranteed */
        for (i = 0; elem2 && (i < pp.len); ++i) {
            /* module check */
            if (pp.pred[i].mod_name) {
                if (strncmp(lyd_node_module(elem2)->name, pp.pred[i].mod_name, pp.pred[i].mod_name_len)
                        || lyd_node_module(elem2)->name[pp.pred[i].mod_name_len]) {
                    break;
                }
            } else {
                if (lyd_node_module(elem2) != lys_node_module(pp.schema)) {
                    break;
                }
            }

            /* name check */
            if (strncmp(elem2->schema->name, pp.pred[i].name, pp.pred[i].nam_len) || elem2->schema->name[pp.pred[i].nam_len]) {
                break;
            }

            /* value check */
            str = ((struct lyd_node_leaf_list *)elem2)->value_str;
            if (strncmp(str, pp.pred[i].value, pp.pred[i].val_len) || str[pp.pred[i].val_len]) {
                break;
            }

            /* next key */
            elem2 = elem2->next;
        }
        if (i == pp.len) {
            return 1;
        }
        return 0;
    default:
        break;
    }

    LOGINT(val2->schema->module->ctx);
    return 0;
}