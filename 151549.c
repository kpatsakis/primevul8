resolve_json_nodeid(const char *nodeid, struct ly_ctx *ctx, const struct lys_node *start, int output)
{
    char *str;
    const char *name, *mod_name, *id, *backup_mod_name = NULL, *yang_data_name = NULL;
    const struct lys_node *sibling, *start_parent, *parent;
    int r, nam_len, mod_name_len, is_relative = -1, has_predicate;
    int yang_data_name_len, backup_mod_name_len;
    /* resolved import module from the start module, it must match the next node-name-match sibling */
    const struct lys_module *prefix_mod, *module, *prev_mod;

    assert(nodeid && (ctx || start));
    if (!ctx) {
        ctx = start->module->ctx;
    }

    id = nodeid;

    if ((r = parse_schema_nodeid(id, &mod_name, &mod_name_len, &name, &nam_len, &is_relative, NULL, NULL, 1)) < 1) {
        LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, id[-r], &id[-r]);
        return NULL;
    }

    if (name[0] == '#') {
        if (is_relative) {
            LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, '#', name);
            return NULL;
        }
        yang_data_name = name + 1;
        yang_data_name_len = nam_len - 1;
        backup_mod_name = mod_name;
        backup_mod_name_len = mod_name_len;
        id += r;
    } else {
        is_relative = -1;
    }

    if ((r = parse_schema_nodeid(id, &mod_name, &mod_name_len, &name, &nam_len, &is_relative, &has_predicate, NULL, 0)) < 1) {
        LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, id[-r], &id[-r]);
        return NULL;
    }
    id += r;

    if (backup_mod_name) {
        mod_name = backup_mod_name;
        mod_name_len = backup_mod_name_len;
    }

    if (is_relative) {
        assert(start);
        start_parent = start;
        while (start_parent && (start_parent->nodetype == LYS_USES)) {
            start_parent = lys_parent(start_parent);
        }
        module = start->module;
    } else {
        if (!mod_name) {
            str = strndup(nodeid, (name + nam_len) - nodeid);
            LOGVAL(ctx, LYE_PATH_MISSMOD, LY_VLOG_STR, nodeid);
            free(str);
            return NULL;
        }

        str = strndup(mod_name, mod_name_len);
        module = ly_ctx_get_module(ctx, str, NULL, 1);
        free(str);

        if (!module) {
            str = strndup(nodeid, (mod_name + mod_name_len) - nodeid);
            LOGVAL(ctx, LYE_PATH_INMOD, LY_VLOG_STR, str);
            free(str);
            return NULL;
        }
        start_parent = NULL;
        if (yang_data_name) {
            start_parent = lyp_get_yang_data_template(module, yang_data_name, yang_data_name_len);
            if (!start_parent) {
                str = strndup(nodeid, (yang_data_name + yang_data_name_len) - nodeid);
                LOGVAL(ctx, LYE_PATH_INNODE, LY_VLOG_STR, str);
                free(str);
                return NULL;
            }
        }

        /* now it's as if there was no module name */
        mod_name = NULL;
        mod_name_len = 0;
    }

    prev_mod = module;

    while (1) {
        sibling = NULL;
        while ((sibling = lys_getnext(sibling, start_parent, module, 0))) {
            /* name match */
            if (sibling->name && !strncmp(name, sibling->name, nam_len) && !sibling->name[nam_len]) {
                /* output check */
                for (parent = lys_parent(sibling); parent && !(parent->nodetype & (LYS_INPUT | LYS_OUTPUT)); parent = lys_parent(parent));
                if (parent) {
                    if (output && (parent->nodetype == LYS_INPUT)) {
                        continue;
                    } else if (!output && (parent->nodetype == LYS_OUTPUT)) {
                        continue;
                    }
                }

                /* module check */
                if (mod_name) {
                    /* will also find an augment module */
                    prefix_mod = ly_ctx_nget_module(ctx, mod_name, mod_name_len, NULL, 1);

                    if (!prefix_mod) {
                        str = strndup(nodeid, (mod_name + mod_name_len) - nodeid);
                        LOGVAL(ctx, LYE_PATH_INMOD, LY_VLOG_STR, str);
                        free(str);
                        return NULL;
                    }
                } else {
                    prefix_mod = prev_mod;
                }
                if (prefix_mod != lys_node_module(sibling)) {
                    continue;
                }

                /* do we have some predicates on it? */
                if (has_predicate) {
                    r = 0;
                    if (sibling->nodetype & (LYS_LEAF | LYS_LEAFLIST)) {
                        if ((r = parse_schema_json_predicate(id, NULL, NULL, NULL, NULL, NULL, NULL, &has_predicate)) < 1) {
                            LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, id[-r], &id[-r]);
                            return NULL;
                        }
                    } else if (sibling->nodetype == LYS_LIST) {
                        if (resolve_json_schema_list_predicate(id, (const struct lys_node_list *)sibling, &r)) {
                            return NULL;
                        }
                    } else {
                        LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, id[0], id);
                        return NULL;
                    }
                    id += r;
                }

                /* the result node? */
                if (!id[0]) {
                    return sibling;
                }

                /* move down the tree, if possible */
                if (sibling->nodetype & (LYS_LEAF | LYS_LEAFLIST | LYS_ANYDATA)) {
                    LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, id[0], id);
                    return NULL;
                }
                start_parent = sibling;

                /* update prev mod */
                prev_mod = (start_parent->child ? lys_node_module(start_parent->child) : module);
                break;
            }
        }

        /* no match */
        if (!sibling) {
            str = strndup(nodeid, (name + nam_len) - nodeid);
            LOGVAL(ctx, LYE_PATH_INNODE, LY_VLOG_STR, str);
            free(str);
            return NULL;
        }

        if ((r = parse_schema_nodeid(id, &mod_name, &mod_name_len, &name, &nam_len, &is_relative, &has_predicate, NULL, 0)) < 1) {
            LOGVAL(ctx, LYE_PATH_INCHAR, LY_VLOG_NONE, NULL, id[-r], &id[-r]);
            return NULL;
        }
        id += r;
    }

    /* cannot get here */
    LOGINT(ctx);
    return NULL;
}