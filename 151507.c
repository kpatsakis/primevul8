resolve_when(struct lyd_node *node, int ignore_fail, struct lys_when **failed_when)
{
    struct lyd_node *ctx_node = NULL, *unlinked_nodes, *tmp_node;
    struct lys_node *sparent;
    struct lyxp_set set;
    enum lyxp_node_type ctx_node_type;
    struct ly_ctx *ctx = node->schema->module->ctx;
    int rc = 0;

    assert(node);
    memset(&set, 0, sizeof set);

    if (!(node->schema->nodetype & (LYS_NOTIF | LYS_RPC | LYS_ACTION)) && snode_get_when(node->schema)) {
        /* make the node dummy for the evaluation */
        node->validity |= LYD_VAL_INUSE;
        rc = lyxp_eval(snode_get_when(node->schema)->cond, node, LYXP_NODE_ELEM, lyd_node_module(node),
                       &set, LYXP_WHEN);
        node->validity &= ~LYD_VAL_INUSE;
        if (rc) {
            if (rc == 1) {
                LOGVAL(ctx, LYE_INWHEN, LY_VLOG_LYD, node, snode_get_when(node->schema)->cond);
            }
            goto cleanup;
        }

        /* set boolean result of the condition */
        lyxp_set_cast(&set, LYXP_SET_BOOLEAN, node, lyd_node_module(node), LYXP_WHEN);
        if (!set.val.bool) {
            node->when_status |= LYD_WHEN_FALSE;
            if ((ignore_fail == 1) || ((snode_get_when(node->schema)->flags & (LYS_XPCONF_DEP | LYS_XPSTATE_DEP))
                    && (ignore_fail == 2))) {
                LOGVRB("When condition \"%s\" is not satisfied, but it is not required.", snode_get_when(node->schema)->cond);
            } else {
                LOGVAL(ctx, LYE_NOWHEN, LY_VLOG_LYD, node, snode_get_when(node->schema)->cond);
                if (failed_when) {
                    *failed_when = snode_get_when(node->schema);
                }
                goto cleanup;
            }
        }

        /* free xpath set content */
        lyxp_set_cast(&set, LYXP_SET_EMPTY, node, lyd_node_module(node), 0);
    }

    sparent = node->schema;
    goto check_augment;

    /* check when in every schema node that affects node */
    while (sparent && (sparent->nodetype & (LYS_USES | LYS_CHOICE | LYS_CASE))) {
        if (snode_get_when(sparent)) {
            if (!ctx_node) {
                rc = resolve_when_ctx_node(node, sparent, &ctx_node, &ctx_node_type);
                if (rc) {
                    LOGINT(ctx);
                    goto cleanup;
                }
            }

            unlinked_nodes = NULL;
            /* we do not want our node pointer to change */
            tmp_node = node;
            rc = resolve_when_unlink_nodes(sparent, &tmp_node, &ctx_node, ctx_node_type, &unlinked_nodes);
            if (rc) {
                goto cleanup;
            }

            rc = lyxp_eval(snode_get_when(sparent)->cond, ctx_node, ctx_node_type, lys_node_module(sparent),
                           &set, LYXP_WHEN);

            if (unlinked_nodes && ctx_node) {
                if (resolve_when_relink_nodes(ctx_node, unlinked_nodes, ctx_node_type)) {
                    rc = -1;
                    goto cleanup;
                }
            }

            if (rc) {
                if (rc == 1) {
                    LOGVAL(ctx, LYE_INWHEN, LY_VLOG_LYD, node, snode_get_when(sparent)->cond);
                }
                goto cleanup;
            }

            lyxp_set_cast(&set, LYXP_SET_BOOLEAN, ctx_node, lys_node_module(sparent), LYXP_WHEN);
            if (!set.val.bool) {
                if ((ignore_fail == 1) || ((snode_get_when(sparent)->flags & (LYS_XPCONF_DEP | LYS_XPSTATE_DEP))
                        && (ignore_fail == 2))) {
                    LOGVRB("When condition \"%s\" is not satisfied, but it is not required.", snode_get_when(sparent)->cond);
                } else {
                    node->when_status |= LYD_WHEN_FALSE;
                    LOGVAL(ctx, LYE_NOWHEN, LY_VLOG_LYD, node, snode_get_when(sparent)->cond);
                    if (failed_when) {
                        *failed_when = snode_get_when(sparent);
                    }
                    goto cleanup;
                }
            }

            /* free xpath set content */
            lyxp_set_cast(&set, LYXP_SET_EMPTY, ctx_node, lys_node_module(sparent), 0);
        }

check_augment:
        if ((sparent->parent && (sparent->parent->nodetype == LYS_AUGMENT) && snode_get_when(sparent->parent))) {
            if (!ctx_node) {
                rc = resolve_when_ctx_node(node, sparent->parent, &ctx_node, &ctx_node_type);
                if (rc) {
                    LOGINT(ctx);
                    goto cleanup;
                }
            }

            unlinked_nodes = NULL;
            tmp_node = node;
            rc = resolve_when_unlink_nodes(sparent->parent, &tmp_node, &ctx_node, ctx_node_type, &unlinked_nodes);
            if (rc) {
                goto cleanup;
            }

            rc = lyxp_eval(snode_get_when(sparent->parent)->cond, ctx_node, ctx_node_type,
                           lys_node_module(sparent->parent), &set, LYXP_WHEN);

            /* reconnect nodes, if ctx_node is NULL then all the nodes were unlinked, but linked together,
             * so the tree did not actually change and there is nothing for us to do
             */
            if (unlinked_nodes && ctx_node) {
                if (resolve_when_relink_nodes(ctx_node, unlinked_nodes, ctx_node_type)) {
                    rc = -1;
                    goto cleanup;
                }
            }

            if (rc) {
                if (rc == 1) {
                    LOGVAL(ctx, LYE_INWHEN, LY_VLOG_LYD, node, snode_get_when(sparent->parent)->cond);
                }
                goto cleanup;
            }

            lyxp_set_cast(&set, LYXP_SET_BOOLEAN, ctx_node, lys_node_module(sparent->parent), LYXP_WHEN);
            if (!set.val.bool) {
                node->when_status |= LYD_WHEN_FALSE;
                if ((ignore_fail == 1) || ((snode_get_when(sparent->parent)->flags & (LYS_XPCONF_DEP | LYS_XPSTATE_DEP))
                        && (ignore_fail == 2))) {
                    LOGVRB("When condition \"%s\" is not satisfied, but it is not required.",
                           snode_get_when(sparent->parent)->cond);
                } else {
                    LOGVAL(ctx, LYE_NOWHEN, LY_VLOG_LYD, node, snode_get_when(sparent->parent)->cond);
                    if (failed_when) {
                        *failed_when = snode_get_when(sparent->parent);
                    }
                    goto cleanup;
                }
            }

            /* free xpath set content */
            lyxp_set_cast(&set, LYXP_SET_EMPTY, ctx_node, lys_node_module(sparent->parent), 0);
        }

        sparent = lys_parent(sparent);
    }

    node->when_status |= LYD_WHEN_TRUE;

cleanup:
    /* free xpath set content */
    lyxp_set_cast(&set, LYXP_SET_EMPTY, ctx_node ? ctx_node : node, NULL, 0);
    return rc;
}