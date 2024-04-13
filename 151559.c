resolve_unique(struct lys_node *parent, const char *uniq_str_path, uint8_t *trg_type)
{
    int rc;
    const struct lys_node *leaf = NULL;
    struct ly_ctx *ctx = parent->module->ctx;

    rc = resolve_descendant_schema_nodeid(uniq_str_path, *lys_child(parent, LYS_LEAF), LYS_LEAF, 1, &leaf);
    if (rc || !leaf) {
        if (rc) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_LYS, parent, uniq_str_path, "unique");
            if (rc > 0) {
                LOGVAL(ctx, LYE_INCHAR, LY_VLOG_PREV, NULL, uniq_str_path[rc - 1], &uniq_str_path[rc - 1]);
            } else if (rc == -2) {
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL, "Unique argument references list.");
            }
            rc = -1;
        } else {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_LYS, parent, uniq_str_path, "unique");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL, "Target leaf not found.");
            rc = EXIT_FAILURE;
        }
        goto error;
    }
    if (leaf->nodetype != LYS_LEAF) {
        LOGVAL(ctx, LYE_INARG, LY_VLOG_LYS, parent, uniq_str_path, "unique");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL, "Target is not a leaf.");
        return -1;
    }

    /* check status */
    if (parent->nodetype != LYS_EXT && lyp_check_status(parent->flags, parent->module, parent->name,
                                                        leaf->flags, leaf->module, leaf->name, leaf)) {
        return -1;
    }

    /* check that all unique's targets are of the same config type */
    if (*trg_type) {
        if (((*trg_type == 1) && (leaf->flags & LYS_CONFIG_R)) || ((*trg_type == 2) && (leaf->flags & LYS_CONFIG_W))) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_LYS, parent, uniq_str_path, "unique");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL,
                   "Leaf \"%s\" referenced in unique statement is config %s, but previous referenced leaf is config %s.",
                   uniq_str_path, *trg_type == 1 ? "false" : "true", *trg_type == 1 ? "true" : "false");
            return -1;
        }
    } else {
        /* first unique */
        if (leaf->flags & LYS_CONFIG_W) {
            *trg_type = 1;
        } else {
            *trg_type = 2;
        }
    }

    /* set leaf's unique flag */
    ((struct lys_node_leaf *)leaf)->flags |= LYS_UNIQUE;

    return EXIT_SUCCESS;

error:

    return rc;
}