valequal(struct lys_node *node, const char *noncan_val, int noncan_val_len, const char *can_val)
{
    int ret;
    struct lyd_node_leaf_list leaf;
    struct lys_node_leaf *sleaf = (struct lys_node_leaf*)node;

    /* dummy leaf */
    memset(&leaf, 0, sizeof leaf);
    leaf.value_str = lydict_insert(node->module->ctx, noncan_val, noncan_val_len);

repeat:
    leaf.value_type = sleaf->type.base;
    leaf.schema = node;

    if (leaf.value_type == LY_TYPE_LEAFREF) {
        if (!sleaf->type.info.lref.target) {
            /* it should either be unresolved leafref (leaf.value_type are ORed flags) or it will be resolved */
            LOGINT(node->module->ctx);
            ret = -1;
            goto finish;
        }
        sleaf = sleaf->type.info.lref.target;
        goto repeat;
    } else {
        if (!lyp_parse_value(&sleaf->type, &leaf.value_str, NULL, &leaf, NULL, NULL, 0, 0, 0)) {
            ret = -1;
            goto finish;
        }
    }

    if (!strcmp(leaf.value_str, can_val)) {
        ret = 1;
    } else {
        ret = 0;
    }

finish:
    lydict_remove(node->module->ctx, leaf.value_str);
    return ret;
}