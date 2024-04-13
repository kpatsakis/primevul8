inherit_config_flag(struct lys_node *node, int flags, int clear)
{
    struct lys_node_leaf *leaf;
    struct ly_ctx *ctx;

    if (!node) {
        return 0;
    }

    assert(!(flags ^ (flags & LYS_CONFIG_MASK)));
    ctx = node->module->ctx;

    LY_TREE_FOR(node, node) {
        if (clear) {
            node->flags &= ~LYS_CONFIG_MASK;
            node->flags &= ~LYS_CONFIG_SET;
        } else {
            if (node->flags & LYS_CONFIG_SET) {
                /* skip nodes with an explicit config value */
                if ((flags & LYS_CONFIG_R) && (node->flags & LYS_CONFIG_W)) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_LYS, node, "true", "config");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL, "State nodes cannot have configuration nodes as children.");
                    return -1;
                }
                continue;
            }

            if (!(node->nodetype & (LYS_USES | LYS_GROUPING))) {
                node->flags = (node->flags & ~LYS_CONFIG_MASK) | flags;
                /* check that configuration lists have keys */
                if ((node->nodetype == LYS_LIST) && (node->flags & LYS_CONFIG_W)
                        && !((struct lys_node_list *)node)->keys_size) {
                    LOGVAL(ctx, LYE_MISSCHILDSTMT, LY_VLOG_LYS, node, "key", "list");
                    return -1;
                }
            }
        }
        if (!(node->nodetype & (LYS_LEAF | LYS_LEAFLIST | LYS_ANYDATA))) {
            if (inherit_config_flag(node->child, flags, clear)) {
                return -1;
            }
        } else if (node->nodetype & (LYS_LEAF | LYS_LEAFLIST)) {
            leaf = (struct lys_node_leaf *)node;
            if (check_leafref_config(leaf, &leaf->type)) {
                return -1;
            }
        }
    }

    return 0;
}