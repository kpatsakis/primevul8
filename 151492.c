resolve_feature(const char *feat_name, uint16_t len, const struct lys_node *node, struct lys_feature **feature)
{
    char *str;
    const char *mod_name, *name;
    int mod_name_len, nam_len, i, j;
    const struct lys_module *module;

    assert(feature);

    /* check prefix */
    if ((i = parse_node_identifier(feat_name, &mod_name, &mod_name_len, &name, &nam_len, NULL, 0)) < 1) {
        LOGVAL(node->module->ctx, LYE_INCHAR, LY_VLOG_NONE, NULL, feat_name[-i], &feat_name[-i]);
        return -1;
    }

    module = lyp_get_module(lys_node_module(node), NULL, 0, mod_name, mod_name_len, 0);
    if (!module) {
        /* identity refers unknown data model */
        LOGVAL(node->module->ctx, LYE_INMOD_LEN, LY_VLOG_NONE, NULL, mod_name_len, mod_name);
        return -1;
    }

    if (module != node->module && module == lys_node_module(node)) {
        /* first, try to search directly in submodule where the feature was mentioned */
        for (j = 0; j < node->module->features_size; j++) {
            if (!strncmp(name, node->module->features[j].name, nam_len) && !node->module->features[j].name[nam_len]) {
                /* check status */
                if (lyp_check_status(node->flags, lys_node_module(node), node->name, node->module->features[j].flags,
                                     node->module->features[j].module, node->module->features[j].name, NULL)) {
                    return -1;
                }
                *feature = &node->module->features[j];
                return 0;
            }
        }
    }

    /* search in the identified module ... */
    for (j = 0; j < module->features_size; j++) {
        if (!strncmp(name, module->features[j].name, nam_len) && !module->features[j].name[nam_len]) {
            /* check status */
            if (lyp_check_status(node->flags, lys_node_module(node), node->name, module->features[j].flags,
                                 module->features[j].module, module->features[j].name, NULL)) {
                return -1;
            }
            *feature = &module->features[j];
            return 0;
        }
    }
    /* ... and all its submodules */
    for (i = 0; i < module->inc_size && module->inc[i].submodule; i++) {
        for (j = 0; j < module->inc[i].submodule->features_size; j++) {
            if (!strncmp(name, module->inc[i].submodule->features[j].name, nam_len)
                    && !module->inc[i].submodule->features[j].name[nam_len]) {
                /* check status */
                if (lyp_check_status(node->flags, lys_node_module(node), node->name,
                                     module->inc[i].submodule->features[j].flags,
                                     module->inc[i].submodule->features[j].module,
                                     module->inc[i].submodule->features[j].name, NULL)) {
                    return -1;
                }
                *feature = &module->inc[i].submodule->features[j];
                return 0;
            }
        }
    }

    /* not found */
    str = strndup(feat_name, len);
    LOGVAL(node->module->ctx, LYE_INRESOLV, LY_VLOG_NONE, NULL, "feature", str);
    free(str);
    return 1;
}