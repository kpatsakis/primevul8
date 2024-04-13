resolve_unres_schema_item(struct lys_module *mod, void *item, enum UNRES_ITEM type, void *str_snode,
                          struct unres_schema *unres)
{
    /* has_str - whether the str_snode is a string in a dictionary that needs to be freed */
    int rc = -1, has_str = 0, parent_type = 0, i, k;
    unsigned int j;
    struct ly_ctx * ctx = mod->ctx;
    struct lys_node *root, *next, *node, *par_grp;
    const char *expr;
    uint8_t *u;

    struct ly_set *refs, *procs;
    struct lys_feature *ref, *feat;
    struct lys_ident *ident;
    struct lys_type *stype;
    struct lys_node_choice *choic;
    struct lyxml_elem *yin;
    struct yang_type *yang;
    struct unres_list_uniq *unique_info;
    struct unres_iffeat_data *iff_data;
    struct unres_ext *ext_data;
    struct lys_ext_instance *ext, **extlist;
    struct lyext_plugin *eplugin;

    switch (type) {
    case UNRES_IDENT:
        expr = str_snode;
        has_str = 1;
        ident = item;

        rc = resolve_base_ident(mod, ident, expr, "identity", NULL, unres);
        break;
    case UNRES_TYPE_IDENTREF:
        expr = str_snode;
        has_str = 1;
        stype = item;

        rc = resolve_base_ident(mod, NULL, expr, "type", stype, unres);
        break;
    case UNRES_TYPE_LEAFREF:
        node = str_snode;
        stype = item;

        rc = resolve_schema_leafref(stype, node, unres);
        break;
    case UNRES_TYPE_DER_EXT:
        parent_type++;
        /* falls through */
    case UNRES_TYPE_DER_TPDF:
        parent_type++;
        /* falls through */
    case UNRES_TYPE_DER:
        /* parent */
        node = str_snode;
        stype = item;

        /* HACK type->der is temporarily unparsed type statement */
        yin = (struct lyxml_elem *)stype->der;
        stype->der = NULL;

        if (yin->flags & LY_YANG_STRUCTURE_FLAG) {
            yang = (struct yang_type *)yin;
            rc = yang_check_type(mod, node, yang, stype, parent_type, unres);

            if (rc) {
                /* may try again later */
                stype->der = (struct lys_tpdf *)yang;
            } else {
                /* we need to always be able to free this, it's safe only in this case */
                lydict_remove(ctx, yang->name);
                free(yang);
            }

        } else {
            rc = fill_yin_type(mod, node, yin, stype, parent_type, unres);
            if (!rc || rc == -1) {
                /* we need to always be able to free this, it's safe only in this case */
                lyxml_free(ctx, yin);
            } else {
                /* may try again later, put all back how it was */
                stype->der = (struct lys_tpdf *)yin;
            }
        }
        if (rc == EXIT_SUCCESS) {
            /* it does not make sense to have leaf-list of empty type */
            if (!parent_type && node->nodetype == LYS_LEAFLIST && stype->base == LY_TYPE_EMPTY) {
                LOGWRN(ctx, "The leaf-list \"%s\" is of \"empty\" type, which does not make sense.", node->name);
            }

            if ((type == UNRES_TYPE_DER_TPDF) && (stype->base == LY_TYPE_UNION)) {
                /* fill typedef union leafref flag */
                ((struct lys_tpdf *)stype->parent)->has_union_leafref = check_type_union_leafref(stype);
            } else if ((type == UNRES_TYPE_DER) && stype->der->has_union_leafref) {
                /* copy the type in case it has union leafref flag */
                if (lys_copy_union_leafrefs(mod, node, stype, NULL, unres)) {
                    LOGERR(ctx, LY_EINT, "Failed to duplicate type.");
                    return -1;
                }
            }
        } else if (rc == EXIT_FAILURE && !(stype->value_flags & LY_VALUE_UNRESGRP)) {
            /* forward reference - in case the type is in grouping, we have to make the grouping unusable
             * by uses statement until the type is resolved. We do that the same way as uses statements inside
             * grouping. The grouping cannot be used unless the unres counter is 0.
             * To remember that the grouping already increased the counter, the LYTYPE_GRP is used as value
             * of the type's base member. */
            for (par_grp = node; par_grp && (par_grp->nodetype != LYS_GROUPING); par_grp = lys_parent(par_grp));
            if (par_grp) {
                if (++((struct lys_node_grp *)par_grp)->unres_count == 0) {
                    LOGERR(ctx, LY_EINT, "Too many unresolved items (type) inside a grouping.");
                    return -1;
                }
                stype->value_flags |= LY_VALUE_UNRESGRP;
            }
        }
        break;
    case UNRES_IFFEAT:
        iff_data = str_snode;
        rc = resolve_feature(iff_data->fname, strlen(iff_data->fname), iff_data->node, item);
        if (!rc) {
            /* success */
            if (iff_data->infeature) {
                /* store backlink into the target feature to allow reverse changes in case of changing feature status */
                feat = *((struct lys_feature **)item);
                if (!feat->depfeatures) {
                    feat->depfeatures = ly_set_new();
                }
                ly_set_add(feat->depfeatures, iff_data->node, LY_SET_OPT_USEASLIST);
            }
            /* cleanup temporary data */
            lydict_remove(ctx, iff_data->fname);
            free(iff_data);
        }
        break;
    case UNRES_FEATURE:
        feat = (struct lys_feature *)item;

        if (feat->iffeature_size) {
            refs = ly_set_new();
            procs = ly_set_new();
            ly_set_add(procs, feat, 0);

            while (procs->number) {
                ref = procs->set.g[procs->number - 1];
                ly_set_rm_index(procs, procs->number - 1);

                for (i = 0; i < ref->iffeature_size; i++) {
                    resolve_iffeature_getsizes(&ref->iffeature[i], NULL, &j);
                    for (; j > 0 ; j--) {
                        if (ref->iffeature[i].features[j - 1]) {
                            if (ref->iffeature[i].features[j - 1] == feat) {
                                LOGVAL(ctx, LYE_CIRC_FEATURES, LY_VLOG_NONE, NULL, feat->name);
                                goto featurecheckdone;
                            }

                            if (ref->iffeature[i].features[j - 1]->iffeature_size) {
                                k = refs->number;
                                if (ly_set_add(refs, ref->iffeature[i].features[j - 1], 0) == k) {
                                    /* not yet seen feature, add it for processing */
                                    ly_set_add(procs, ref->iffeature[i].features[j - 1], 0);
                                }
                            }
                        } else {
                            /* forward reference */
                            rc = EXIT_FAILURE;
                            goto featurecheckdone;
                        }
                    }

                }
            }
            rc = EXIT_SUCCESS;

featurecheckdone:
            ly_set_free(refs);
            ly_set_free(procs);
        }

        break;
    case UNRES_USES:
        rc = resolve_unres_schema_uses(item, unres);
        break;
    case UNRES_TYPEDEF_DFLT:
        parent_type++;
        /* falls through */
    case UNRES_TYPE_DFLT:
        stype = item;
        rc = check_default(stype, (const char **)str_snode, mod, parent_type);
        if ((rc == EXIT_FAILURE) && !parent_type && (stype->base == LY_TYPE_LEAFREF)) {
            for (par_grp = (struct lys_node *)stype->parent;
                 par_grp && (par_grp->nodetype != LYS_GROUPING);
                 par_grp = lys_parent(par_grp));
            if (par_grp) {
                /* checking default value in a grouping finished with forward reference means we cannot check the value */
                rc = EXIT_SUCCESS;
            }
        }
        break;
    case UNRES_CHOICE_DFLT:
        expr = str_snode;
        has_str = 1;
        choic = item;

        if (!choic->dflt) {
            choic->dflt = resolve_choice_dflt(choic, expr);
        }
        if (choic->dflt) {
            rc = lyp_check_mandatory_choice((struct lys_node *)choic);
        } else {
            rc = EXIT_FAILURE;
        }
        break;
    case UNRES_LIST_KEYS:
        rc = resolve_list_keys(item, ((struct lys_node_list *)item)->keys_str);
        break;
    case UNRES_LIST_UNIQ:
        unique_info = (struct unres_list_uniq *)item;
        rc = resolve_unique(unique_info->list, unique_info->expr, unique_info->trg_type);
        break;
    case UNRES_AUGMENT:
        rc = resolve_augment(item, NULL, unres);
        break;
    case UNRES_XPATH:
        node = (struct lys_node *)item;
        rc = check_xpath(node, 1);
        break;
    case UNRES_MOD_IMPLEMENT:
        rc = lys_make_implemented_r(mod, unres);
        break;
    case UNRES_EXT:
        ext_data = (struct unres_ext *)str_snode;
        extlist = &(*(struct lys_ext_instance ***)item)[ext_data->ext_index];
        rc = resolve_extension(ext_data, extlist, unres);
        if (!rc) {
            /* success */
            /* is there a callback to be done to finalize the extension? */
            eplugin = extlist[0]->def->plugin;
            if (eplugin) {
                if (eplugin->check_result || (eplugin->flags & LYEXT_OPT_INHERIT)) {
                    u = malloc(sizeof *u);
                    LY_CHECK_ERR_RETURN(!u, LOGMEM(ctx), -1);
                    (*u) = ext_data->ext_index;
                    if (unres_schema_add_node(mod, unres, item, UNRES_EXT_FINALIZE, (struct lys_node *)u) == -1) {
                        /* something really bad happend since the extension finalization is not actually
                         * being resolved while adding into unres, so something more serious with the unres
                         * list itself must happened */
                        return -1;
                    }
                }
            }
        }
        if (!rc || rc == -1) {
            /* cleanup on success or fatal error */
            if (ext_data->datatype == LYS_IN_YIN) {
                /* YIN */
                lyxml_free(ctx, ext_data->data.yin);
            } else {
                /* YANG */
                yang_free_ext_data(ext_data->data.yang);
            }
            free(ext_data);
        }
        break;
    case UNRES_EXT_FINALIZE:
        u = (uint8_t *)str_snode;
        ext = (*(struct lys_ext_instance ***)item)[*u];
        free(u);

        eplugin = ext->def->plugin;

        /* inherit */
        if ((eplugin->flags & LYEXT_OPT_INHERIT) && (ext->parent_type == LYEXT_PAR_NODE)) {
            root = (struct lys_node *)ext->parent;
            if (!(root->nodetype & (LYS_LEAF | LYS_LEAFLIST | LYS_ANYDATA))) {
                LY_TREE_DFS_BEGIN(root->child, next, node) {
                    /* first, check if the node already contain instance of the same extension,
                     * in such a case we won't inherit. In case the node was actually defined as
                     * augment data, we are supposed to check the same way also the augment node itself */
                    if (lys_ext_instance_presence(ext->def, node->ext, node->ext_size) != -1) {
                        goto inherit_dfs_sibling;
                    } else if (node->parent != root && node->parent->nodetype == LYS_AUGMENT &&
                            lys_ext_instance_presence(ext->def, node->parent->ext, node->parent->ext_size) != -1) {
                        goto inherit_dfs_sibling;
                    }

                    if (eplugin->check_inherit) {
                        /* we have a callback to check the inheritance, use it */
                        switch ((rc = (*eplugin->check_inherit)(ext, node))) {
                        case 0:
                            /* yes - continue with the inheriting code */
                            break;
                        case 1:
                            /* no - continue with the node's sibling */
                            goto inherit_dfs_sibling;
                        case 2:
                            /* no, but continue with the children, just skip the inheriting code for this node */
                            goto inherit_dfs_child;
                        default:
                            LOGERR(ctx, LY_EINT, "Plugin's (%s:%s) check_inherit callback returns invalid value (%d),",
                                   ext->def->module->name, ext->def->name, rc);
                        }
                    }

                    /* inherit the extension */
                    extlist = realloc(node->ext, (node->ext_size + 1) * sizeof *node->ext);
                    LY_CHECK_ERR_RETURN(!extlist, LOGMEM(ctx), -1);
                    extlist[node->ext_size] = malloc(sizeof **extlist);
                    LY_CHECK_ERR_RETURN(!extlist[node->ext_size], LOGMEM(ctx); node->ext = extlist, -1);
                    memcpy(extlist[node->ext_size], ext, sizeof *ext);
                    extlist[node->ext_size]->flags |= LYEXT_OPT_INHERIT;

                    node->ext = extlist;
                    node->ext_size++;

inherit_dfs_child:
                    /* modification of - select element for the next run - children first */
                    if (node->nodetype & (LYS_LEAF | LYS_LEAFLIST | LYS_ANYDATA)) {
                        next = NULL;
                    } else {
                        next = node->child;
                    }
                    if (!next) {
inherit_dfs_sibling:
                        /* no children, try siblings */
                        next = node->next;
                    }
                    while (!next) {
                        /* go to the parent */
                        node = lys_parent(node);

                        /* we are done if we are back in the root (the starter's parent */
                        if (node == root) {
                            break;
                        }

                        /* parent is already processed, go to its sibling */
                        next = node->next;
                    }
                }
            }
        }

        /* final check */
        if (eplugin->check_result) {
            if ((*eplugin->check_result)(ext)) {
                LOGERR(ctx, LY_EPLUGIN, "Resolving extension failed.");
                return -1;
            }
        }

        rc = 0;
        break;
    default:
        LOGINT(ctx);
        break;
    }

    if (has_str && !rc) {
        /* the string is no more needed in case of success.
         * In case of forward reference, we will try to resolve the string later */
        lydict_remove(ctx, str_snode);
    }

    return rc;
}