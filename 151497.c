resolve_leafref(struct lyd_node_leaf_list *leaf, const char *path, int req_inst, struct lyd_node **ret)
{
    struct lyxp_set xp_set;
    uint32_t i;

    memset(&xp_set, 0, sizeof xp_set);
    *ret = NULL;

    /* syntax was already checked, so just evaluate the path using standard XPath */
    if (lyxp_eval(path, (struct lyd_node *)leaf, LYXP_NODE_ELEM, lyd_node_module((struct lyd_node *)leaf), &xp_set, 0) != EXIT_SUCCESS) {
        return -1;
    }

    if (xp_set.type == LYXP_SET_NODE_SET) {
        for (i = 0; i < xp_set.used; ++i) {
            if ((xp_set.val.nodes[i].type != LYXP_NODE_ELEM) || !(xp_set.val.nodes[i].node->schema->nodetype & (LYS_LEAF | LYS_LEAFLIST))) {
                continue;
            }

            /* not that the value is already in canonical form since the parsers does the conversion,
             * so we can simply compare just the values */
            if (ly_strequal(leaf->value_str, ((struct lyd_node_leaf_list *)xp_set.val.nodes[i].node)->value_str, 1)) {
                /* we have the match */
                *ret = xp_set.val.nodes[i].node;
                break;
            }
        }
    }

    lyxp_set_cast(&xp_set, LYXP_SET_EMPTY, (struct lyd_node *)leaf, NULL, 0);

    if (!*ret) {
        /* reference not found */
        if (req_inst > -1) {
            LOGVAL(leaf->schema->module->ctx, LYE_NOLEAFREF, LY_VLOG_LYD, leaf, path, leaf->value_str);
            return EXIT_FAILURE;
        } else {
            LOGVRB("There is no leafref \"%s\" with the value \"%s\", but it is not required.", path, leaf->value_str);
        }
    }

    return EXIT_SUCCESS;
}