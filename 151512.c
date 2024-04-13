check_leafref_features(struct lys_type *type)
{
    struct lys_node *iter;
    struct ly_set *src_parents, *trg_parents, *features;
    struct lys_node_augment *aug;
    struct ly_ctx *ctx = ((struct lys_tpdf *)type->parent)->module->ctx;
    unsigned int i, j, size, x;
    int ret = EXIT_SUCCESS;

    assert(type->parent);

    src_parents = ly_set_new();
    trg_parents = ly_set_new();
    features = ly_set_new();

    /* get parents chain of source (leafref) */
    for (iter = (struct lys_node *)type->parent; iter; iter = lys_parent(iter)) {
        if (iter->nodetype & (LYS_INPUT | LYS_OUTPUT)) {
            continue;
        }
        if (iter->parent && (iter->parent->nodetype == LYS_AUGMENT)) {
            aug = (struct lys_node_augment *)iter->parent;
            if ((aug->module->implemented && (aug->flags & LYS_NOTAPPLIED)) || !aug->target) {
                /* unresolved augment, wait until it's resolved */
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_LYS, aug,
                       "Cannot check leafref \"%s\" if-feature consistency because of an unresolved augment.", type->info.lref.path);
                ret = EXIT_FAILURE;
                goto cleanup;
            }
            /* also add this augment */
            ly_set_add(src_parents, aug, LY_SET_OPT_USEASLIST);
        }
        ly_set_add(src_parents, iter, LY_SET_OPT_USEASLIST);
    }
    /* get parents chain of target */
    for (iter = (struct lys_node *)type->info.lref.target; iter; iter = lys_parent(iter)) {
        if (iter->nodetype & (LYS_INPUT | LYS_OUTPUT)) {
            continue;
        }
        if (iter->parent && (iter->parent->nodetype == LYS_AUGMENT)) {
            aug = (struct lys_node_augment *)iter->parent;
            if ((aug->module->implemented && (aug->flags & LYS_NOTAPPLIED)) || !aug->target) {
                /* unresolved augment, wait until it's resolved */
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_LYS, aug,
                       "Cannot check leafref \"%s\" if-feature consistency because of an unresolved augment.", type->info.lref.path);
                ret = EXIT_FAILURE;
                goto cleanup;
            }
        }
        ly_set_add(trg_parents, iter, LY_SET_OPT_USEASLIST);
    }

    /* compare the features used in if-feature statements in the rest of both
     * chains of parents. The set of features used for target must be a subset
     * of features used for the leafref. This is not a perfect, we should compare
     * the truth tables but it could require too much resources, so we simplify that */
    for (i = 0; i < src_parents->number; i++) {
        iter = src_parents->set.s[i]; /* shortcut */
        if (!iter->iffeature_size) {
            continue;
        }
        for (j = 0; j < iter->iffeature_size; j++) {
            resolve_iffeature_getsizes(&iter->iffeature[j], NULL, &size);
            for (; size; size--) {
                if (!iter->iffeature[j].features[size - 1]) {
                    /* not yet resolved feature, postpone this check */
                    ret = EXIT_FAILURE;
                    goto cleanup;
                }
                ly_set_add(features, iter->iffeature[j].features[size - 1], 0);
            }
        }
    }
    x = features->number;
    for (i = 0; i < trg_parents->number; i++) {
        iter = trg_parents->set.s[i]; /* shortcut */
        if (!iter->iffeature_size) {
            continue;
        }
        for (j = 0; j < iter->iffeature_size; j++) {
            resolve_iffeature_getsizes(&iter->iffeature[j], NULL, &size);
            for (; size; size--) {
                if (!iter->iffeature[j].features[size - 1]) {
                    /* not yet resolved feature, postpone this check */
                    ret = EXIT_FAILURE;
                    goto cleanup;
                }
                if ((unsigned)ly_set_add(features, iter->iffeature[j].features[size - 1], 0) >= x) {
                    /* the feature is not present in features set of target's parents chain */
                    LOGVAL(ctx, LYE_NORESOLV, LY_VLOG_LYS, type->parent, "leafref", type->info.lref.path);
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL,
                           "Leafref is not conditional based on \"%s\" feature as its target.",
                           iter->iffeature[j].features[size - 1]->name);
                    ret = -1;
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    ly_set_free(features);
    ly_set_free(src_parents);
    ly_set_free(trg_parents);

    return ret;
}