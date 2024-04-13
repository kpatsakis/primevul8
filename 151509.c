check_key(struct lys_node_list *list, int index, const char *name, int len)
{
    struct lys_node_leaf *key = list->keys[index];
    char *dup = NULL;
    int j;
    struct ly_ctx *ctx = list->module->ctx;

    /* existence */
    if (!key) {
        if (name[len] != '\0') {
            dup = strdup(name);
            LY_CHECK_ERR_RETURN(!dup, LOGMEM(ctx), -1);
            dup[len] = '\0';
            name = dup;
        }
        LOGVAL(ctx, LYE_KEY_MISS, LY_VLOG_LYS, list, name);
        free(dup);
        return -1;
    }

    /* uniqueness */
    for (j = index - 1; j >= 0; j--) {
        if (key == list->keys[j]) {
            LOGVAL(ctx, LYE_KEY_DUP, LY_VLOG_LYS, list, key->name);
            return -1;
        }
    }

    /* key is a leaf */
    if (key->nodetype != LYS_LEAF) {
        LOGVAL(ctx, LYE_KEY_NLEAF, LY_VLOG_LYS, list, key->name);
        return -1;
    }

    /* type of the leaf is not built-in empty */
    if (key->type.base == LY_TYPE_EMPTY && key->module->version < LYS_VERSION_1_1) {
        LOGVAL(ctx, LYE_KEY_TYPE, LY_VLOG_LYS, list, key->name);
        return -1;
    }

    /* config attribute is the same as of the list */
    if ((key->flags & LYS_CONFIG_MASK) && (list->flags & LYS_CONFIG_MASK)
            && ((list->flags & LYS_CONFIG_MASK) != (key->flags & LYS_CONFIG_MASK))) {
        LOGVAL(ctx, LYE_KEY_CONFIG, LY_VLOG_LYS, list, key->name);
        return -1;
    }

    /* key is not placed from augment */
    if (key->parent->nodetype == LYS_AUGMENT) {
        LOGVAL(ctx, LYE_KEY_MISS, LY_VLOG_LYS, key, key->name);
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL, "Key inserted from augment.");
        return -1;
    }

    /* key is not when/if-feature -conditional */
    j = 0;
    if (key->when || (key->iffeature_size && (j = 1))) {
        LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_LYS, key, j ? "if-feature" : "when", "leaf");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL, "Key definition cannot depend on a \"%s\" condition.",
               j ? "if-feature" : "when");
        return -1;
    }

    return EXIT_SUCCESS;
}