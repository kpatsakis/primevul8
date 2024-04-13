resolve_schema_nodeid(const char *nodeid, const struct lys_node *start_parent, const struct lys_module *cur_module,
                      struct ly_set **ret, int extended, int no_node_error)
{
    const char *name, *mod_name, *id, *backup_mod_name = NULL, *yang_data_name = NULL;
    const struct lys_node *sibling, *next, *elem;
    struct lys_node_augment *last_aug;
    int r, nam_len, mod_name_len = 0, is_relative = -1, all_desc, has_predicate, nodeid_end = 0;
    int yang_data_name_len, backup_mod_name_len = 0;
    /* resolved import module from the start module, it must match the next node-name-match sibling */
    const struct lys_module *start_mod, *aux_mod = NULL;
    char *str;
    struct ly_ctx *ctx;

    assert(nodeid && (start_parent || cur_module) && ret);
    *ret = NULL;

    if (!cur_module) {
        cur_module = lys_node_module(start_parent);
    }
    ctx = cur_module->ctx;
    id = nodeid;

    r = parse_schema_nodeid(id, &mod_name, &mod_name_len, &name, &nam_len, &is_relative, NULL, NULL, 1);
    if (r < 1) {
        LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, id[r], &id[r]);
        return -1;
    }

    if (name[0] == '#') {
        if (is_relative) {
            LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, '#', name);
            return -1;
        }
        yang_data_name = name + 1;
        yang_data_name_len = nam_len - 1;
        backup_mod_name = mod_name;
        backup_mod_name_len = mod_name_len;
        id += r;
    } else {
        is_relative = -1;
    }

    r = parse_schema_nodeid(id, &mod_name, &mod_name_len, &name, &nam_len, &is_relative, &has_predicate,
                            (extended ? &all_desc : NULL), extended);
    if (r < 1) {
        LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, id[r], &id[r]);
        return -1;
    }
    id += r;

    if (backup_mod_name) {
        mod_name = backup_mod_name;
        mod_name_len = backup_mod_name_len;
    }

    if (is_relative && !start_parent) {
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_STR, nodeid, "Starting node must be provided for relative paths.");
        return -1;
    }

    /* descendant-schema-nodeid */
    if (is_relative) {
        cur_module = start_mod = lys_node_module(start_parent);

    /* absolute-schema-nodeid */
    } else {
        start_mod = lyp_get_module(cur_module, NULL, 0, mod_name, mod_name_len, 0);
        if (!start_mod) {
            str = strndup(mod_name, mod_name_len);
            LOGVAL(ctx, LYE_PATH_INMOD, LY_VLOG_STR, str);
            free(str);
            return -1;
        }
        start_parent = NULL;
        if (yang_data_name) {
            start_parent = lyp_get_yang_data_template(start_mod, yang_data_name, yang_data_name_len);
            if (!start_parent) {
                str = strndup(nodeid, (yang_data_name + yang_data_name_len) - nodeid);
                LOGVAL(ctx, LYE_PATH_INNODE, LY_VLOG_STR, str);
                free(str);
                return -1;
            }
        }
    }

    while (1) {
        sibling = NULL;
        last_aug = NULL;

        if (start_parent) {
            if (mod_name && (strncmp(mod_name, cur_module->name, mod_name_len)
                    || (mod_name_len != (signed)strlen(cur_module->name)))) {
                /* we are getting into another module (augment) */
                aux_mod = lyp_get_module(cur_module, NULL, 0, mod_name, mod_name_len, 0);
                if (!aux_mod) {
                    str = strndup(mod_name, mod_name_len);
                    LOGVAL(ctx, LYE_PATH_INMOD, LY_VLOG_STR, str);
                    free(str);
                    return -1;
                }
            } else {
                /* there is no mod_name, so why are we checking augments again?
                 * because this module may be not implemented and it augments something in another module and
                 * there is another augment augmenting that previous one */
                aux_mod = cur_module;
            }

            /* look into augments */
            if (!extended) {
get_next_augment:
                last_aug = lys_getnext_target_aug(last_aug, aux_mod, start_parent);
            }
        }

        while ((sibling = lys_getnext(sibling, (last_aug ? (struct lys_node *)last_aug : start_parent), start_mod,
                LYS_GETNEXT_WITHCHOICE | LYS_GETNEXT_WITHCASE | LYS_GETNEXT_WITHINOUT | LYS_GETNEXT_PARENTUSES | LYS_GETNEXT_NOSTATECHECK))) {
            r = schema_nodeid_siblingcheck(sibling, cur_module, mod_name, mod_name_len, name, nam_len);

            /* resolve predicate */
            if (extended && ((r == 0) || (r == 2) || (r == 3)) && has_predicate) {
                r = resolve_extended_schema_nodeid_predicate(id, sibling, cur_module, &nodeid_end);
                if (r == 1) {
                    continue;
                } else if (r == -1) {
                    return -1;
                }
            } else if (!id[0]) {
                nodeid_end = 1;
            }

            if (r == 0) {
                /* one matching result */
                if (nodeid_end) {
                    *ret = ly_set_new();
                    LY_CHECK_ERR_RETURN(!*ret, LOGMEM(ctx), -1);
                    ly_set_add(*ret, (void *)sibling, LY_SET_OPT_USEASLIST);
                } else {
                    if (sibling->nodetype & (LYS_LEAF | LYS_LEAFLIST | LYS_ANYDATA)) {
                        return -1;
                    }
                    start_parent = sibling;
                }
                break;
            } else if (r == 1) {
                continue;
            } else if (r == 2) {
                /* "*" */
                if (!*ret) {
                    *ret = ly_set_new();
                    LY_CHECK_ERR_RETURN(!*ret, LOGMEM(ctx), -1);
                }
                ly_set_add(*ret, (void *)sibling, LY_SET_OPT_USEASLIST);
                if (all_desc) {
                    LY_TREE_DFS_BEGIN(sibling, next, elem) {
                        if (elem != sibling) {
                            ly_set_add(*ret, (void *)elem, LY_SET_OPT_USEASLIST);
                        }

                        LY_TREE_DFS_END(sibling, next, elem);
                    }
                }
            } else if (r == 3) {
                /* "." */
                if (!*ret) {
                    *ret = ly_set_new();
                    LY_CHECK_ERR_RETURN(!*ret, LOGMEM(ctx), -1);
                    ly_set_add(*ret, (void *)start_parent, LY_SET_OPT_USEASLIST);
                }
                ly_set_add(*ret, (void *)sibling, LY_SET_OPT_USEASLIST);
                if (all_desc) {
                    LY_TREE_DFS_BEGIN(sibling, next, elem) {
                        if (elem != sibling) {
                            ly_set_add(*ret, (void *)elem, LY_SET_OPT_USEASLIST);
                        }

                        LY_TREE_DFS_END(sibling, next, elem);
                    }
                }
            } else {
                LOGINT(ctx);
                return -1;
            }
        }

        /* skip predicate */
        if (extended && has_predicate) {
            while (id[0] == '[') {
                id = strchr(id, ']');
                if (!id) {
                    LOGINT(ctx);
                    return -1;
                }
                ++id;
            }
        }

        if (nodeid_end && ((r == 0) || (r == 2) || (r == 3))) {
            return EXIT_SUCCESS;
        }

        /* no match */
        if (!sibling) {
            if (last_aug) {
                /* it still could be in another augment */
                goto get_next_augment;
            }
            if (no_node_error) {
                str = strndup(nodeid, (name - nodeid) + nam_len);
                LOGVAL(ctx, LYE_PATH_INNODE, LY_VLOG_STR, str);
                free(str);
                return -1;
            }
            *ret = NULL;
            return EXIT_SUCCESS;
        }

        r = parse_schema_nodeid(id, &mod_name, &mod_name_len, &name, &nam_len, &is_relative, &has_predicate,
                                (extended ? &all_desc : NULL), extended);
        if (r < 1) {
            LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, id[r], &id[r]);
            return -1;
        }
        id += r;
    }

    /* cannot get here */
    LOGINT(ctx);
    return -1;
}