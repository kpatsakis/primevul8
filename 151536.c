resolve_instid(struct lyd_node *data, const char *path, int req_inst, struct lyd_node **ret)
{
    int i = 0, j, parsed, cur_idx;
    const struct lys_module *mod, *prev_mod = NULL;
    struct ly_ctx *ctx = data->schema->module->ctx;
    struct lyd_node *root, *node;
    const char *model = NULL, *name;
    char *str;
    int mod_len, name_len, has_predicate;
    struct unres_data node_match;

    memset(&node_match, 0, sizeof node_match);
    *ret = NULL;

    /* we need root to resolve absolute path */
    for (root = data; root->parent; root = root->parent);
    /* we're still parsing it and the pointer is not correct yet */
    if (root->prev) {
        for (; root->prev->next; root = root->prev);
    }

    /* search for the instance node */
    while (path[i]) {
        j = parse_instance_identifier(&path[i], &model, &mod_len, &name, &name_len, &has_predicate);
        if (j <= 0) {
            LOGVAL(ctx, LYE_INCHAR, LY_VLOG_LYD, data, path[i-j], &path[i-j]);
            goto error;
        }
        i += j;

        if (model) {
            str = strndup(model, mod_len);
            if (!str) {
                LOGMEM(ctx);
                goto error;
            }
            mod = ly_ctx_get_module(ctx, str, NULL, 1);
            if (ctx->data_clb) {
                if (!mod) {
                    mod = ctx->data_clb(ctx, str, NULL, 0, ctx->data_clb_data);
                } else if (!mod->implemented) {
                    mod = ctx->data_clb(ctx, mod->name, mod->ns, LY_MODCLB_NOT_IMPLEMENTED, ctx->data_clb_data);
                }
            }
            free(str);

            if (!mod || !mod->implemented || mod->disabled) {
                break;
            }
        } else if (!prev_mod) {
            /* first iteration and we are missing module name */
            LOGVAL(ctx, LYE_INELEM_LEN, LY_VLOG_LYD, data, name_len, name);
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL, "Instance-identifier is missing prefix in the first node.");
            goto error;
        } else {
            mod = prev_mod;
        }

        if (resolve_data(mod, name, name_len, root, &node_match)) {
            /* no instance exists */
            break;
        }

        if (has_predicate) {
            /* we have predicate, so the current results must be list or leaf-list */
            parsed = j = 0;
            /* index of the current node (for lists with position predicates) */
            cur_idx = 1;
            while (j < (signed)node_match.count) {
                node = node_match.node[j];
                parsed = resolve_instid_predicate(mod, &path[i], &node, cur_idx);
                if (parsed < 1) {
                    LOGVAL(ctx, LYE_INPRED, LY_VLOG_LYD, data, &path[i - parsed]);
                    goto error;
                }

                if (!node) {
                    /* current node does not satisfy the predicate */
                    unres_data_del(&node_match, j);
                } else {
                    ++j;
                }
                ++cur_idx;
            }

            i += parsed;
        } else if (node_match.count) {
            /* check that we are not addressing lists */
            for (j = 0; (unsigned)j < node_match.count; ++j) {
                if (node_match.node[j]->schema->nodetype == LYS_LIST) {
                    unres_data_del(&node_match, j--);
                }
            }
            if (!node_match.count) {
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_LYD, data, "Instance identifier is missing list keys.");
            }
        }

        prev_mod = mod;
    }

    if (!node_match.count) {
        /* no instance exists */
        if (req_inst > -1) {
            LOGVAL(ctx, LYE_NOREQINS, LY_VLOG_LYD, data, path);
            return EXIT_FAILURE;
        }
        LOGVRB("There is no instance of \"%s\", but it is not required.", path);
        return EXIT_SUCCESS;
    } else if (node_match.count > 1) {
        /* instance identifier must resolve to a single node */
        LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_LYD, data, path, "data tree");
        goto error;
    } else {
        /* we have required result, remember it and cleanup */
        *ret = node_match.node[0];
        free(node_match.node);
        return EXIT_SUCCESS;
    }

error:
    /* cleanup */
    free(node_match.node);
    return -1;
}