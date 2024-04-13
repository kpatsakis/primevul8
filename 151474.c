resolve_must(struct lyd_node *node, int inout_parent, int ignore_fail)
{
    uint8_t i, must_size;
    struct lys_node *schema;
    struct lys_restr *must;
    struct lyxp_set set;
    struct ly_ctx *ctx = node->schema->module->ctx;

    assert(node);
    memset(&set, 0, sizeof set);

    if (inout_parent) {
        for (schema = lys_parent(node->schema);
             schema && (schema->nodetype & (LYS_CHOICE | LYS_CASE | LYS_USES));
             schema = lys_parent(schema));
        if (!schema || !(schema->nodetype & (LYS_INPUT | LYS_OUTPUT))) {
            LOGINT(ctx);
            return -1;
        }
        must_size = ((struct lys_node_inout *)schema)->must_size;
        must = ((struct lys_node_inout *)schema)->must;

        /* context node is the RPC/action */
        node = node->parent;
        if (!(node->schema->nodetype & (LYS_RPC | LYS_ACTION))) {
            LOGINT(ctx);
            return -1;
        }
    } else {
        switch (node->schema->nodetype) {
        case LYS_CONTAINER:
            must_size = ((struct lys_node_container *)node->schema)->must_size;
            must = ((struct lys_node_container *)node->schema)->must;
            break;
        case LYS_LEAF:
            must_size = ((struct lys_node_leaf *)node->schema)->must_size;
            must = ((struct lys_node_leaf *)node->schema)->must;
            break;
        case LYS_LEAFLIST:
            must_size = ((struct lys_node_leaflist *)node->schema)->must_size;
            must = ((struct lys_node_leaflist *)node->schema)->must;
            break;
        case LYS_LIST:
            must_size = ((struct lys_node_list *)node->schema)->must_size;
            must = ((struct lys_node_list *)node->schema)->must;
            break;
        case LYS_ANYXML:
        case LYS_ANYDATA:
            must_size = ((struct lys_node_anydata *)node->schema)->must_size;
            must = ((struct lys_node_anydata *)node->schema)->must;
            break;
        case LYS_NOTIF:
            must_size = ((struct lys_node_notif *)node->schema)->must_size;
            must = ((struct lys_node_notif *)node->schema)->must;
            break;
        default:
            must_size = 0;
            break;
        }
    }

    for (i = 0; i < must_size; ++i) {
        if (lyxp_eval(must[i].expr, node, LYXP_NODE_ELEM, lyd_node_module(node), &set, LYXP_MUST)) {
            return -1;
        }

        lyxp_set_cast(&set, LYXP_SET_BOOLEAN, node, lyd_node_module(node), LYXP_MUST);

        if (!set.val.bool) {
            if ((ignore_fail == 1) || ((must[i].flags & (LYS_XPCONF_DEP | LYS_XPSTATE_DEP)) && (ignore_fail == 2))) {
                LOGVRB("Must condition \"%s\" not satisfied, but it is not required.", must[i].expr);
            } else {
                LOGVAL(ctx, LYE_NOMUST, LY_VLOG_LYD, node, must[i].expr);
                if (must[i].emsg) {
                    ly_vlog_str(ctx, LY_VLOG_PREV, must[i].emsg);
                }
                if (must[i].eapptag) {
                    ly_err_last_set_apptag(ctx, must[i].eapptag);
                }
                return 1;
            }
        }
    }

    return EXIT_SUCCESS;
}