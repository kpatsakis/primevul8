resolve_unres_data(struct ly_ctx *ctx, struct unres_data *unres, struct lyd_node **root, int options)
{
    uint32_t i, j, first, resolved, del_items, stmt_count;
    uint8_t prev_when_status;
    int rc, progress, ignore_fail;
    enum int_log_opts prev_ilo;
    struct ly_err_item *prev_eitem;
    LY_ERR prev_ly_errno = ly_errno;
    struct lyd_node *parent;
    struct lys_when *when;

    assert(root);
    assert(unres);

    if (!unres->count) {
        return EXIT_SUCCESS;
    }

    if (options & (LYD_OPT_NOTIF_FILTER | LYD_OPT_GET | LYD_OPT_GETCONFIG | LYD_OPT_EDIT)) {
        ignore_fail = 1;
    } else if (options & LYD_OPT_NOEXTDEPS) {
        ignore_fail = 2;
    } else {
        ignore_fail = 0;
    }

    LOGVRB("Resolving unresolved data nodes and their constraints...");
    if (!ignore_fail) {
        /* remember logging state only if errors are generated and valid */
        ly_ilo_change(ctx, ILO_STORE, &prev_ilo, &prev_eitem);
    }

    /*
     * when-stmt first
     */
    first = 1;
    stmt_count = 0;
    resolved = 0;
    del_items = 0;
    do {
        if (!ignore_fail) {
            ly_err_free_next(ctx, prev_eitem);
        }
        progress = 0;
        for (i = 0; i < unres->count; i++) {
            if (unres->type[i] != UNRES_WHEN) {
                continue;
            }
            if (first) {
                /* count when-stmt nodes in unres list */
                stmt_count++;
            }

            /* resolve when condition only when all parent when conditions are already resolved */
            for (parent = unres->node[i]->parent;
                 parent && LYD_WHEN_DONE(parent->when_status);
                 parent = parent->parent) {
                if (!parent->parent && (parent->when_status & LYD_WHEN_FALSE)) {
                    /* the parent node was already unlinked, do not resolve this node,
                     * it will be removed anyway, so just mark it as resolved
                     */
                    unres->node[i]->when_status |= LYD_WHEN_FALSE;
                    unres->type[i] = UNRES_RESOLVED;
                    resolved++;
                    break;
                }
            }
            if (parent) {
                continue;
            }

            prev_when_status = unres->node[i]->when_status;
            rc = resolve_unres_data_item(unres->node[i], unres->type[i], ignore_fail, &when);
            if (!rc) {
                /* finish with error/delete the node only if when was changed from true to false, an external
                 * dependency was not required, or it was not provided (the flag would not be passed down otherwise,
                 * checked in upper functions) */
                if ((unres->node[i]->when_status & LYD_WHEN_FALSE)
                        && (!(when->flags & (LYS_XPCONF_DEP | LYS_XPSTATE_DEP)) || !(options & LYD_OPT_NOEXTDEPS))) {
                    if ((!(prev_when_status & LYD_WHEN_TRUE) || !(options & LYD_OPT_WHENAUTODEL)) && !unres->node[i]->dflt) {
                        /* false when condition */
                        goto error;
                    } /* follows else */

                    /* auto-delete */
                    LOGVRB("Auto-deleting node \"%s\" due to when condition (%s)", ly_errpath(ctx), when->cond);

                    /* only unlink now, the subtree can contain another nodes stored in the unres list */
                    /* if it has parent non-presence containers that would be empty, we should actually
                     * remove the container
                     */
                    for (parent = unres->node[i];
                            parent->parent && parent->parent->schema->nodetype == LYS_CONTAINER;
                            parent = parent->parent) {
                        if (((struct lys_node_container *)parent->parent->schema)->presence) {
                            /* presence container */
                            break;
                        }
                        if (parent->next || parent->prev != parent) {
                            /* non empty (the child we are in and we are going to remove is not the only child) */
                            break;
                        }
                    }
                    unres->node[i] = parent;

                    if (*root && *root == unres->node[i]) {
                        *root = (*root)->next;
                    }

                    lyd_unlink(unres->node[i]);
                    unres->type[i] = UNRES_DELETE;
                    del_items++;

                    /* update the rest of unres items */
                    for (j = 0; j < unres->count; j++) {
                        if (unres->type[j] == UNRES_RESOLVED || unres->type[j] == UNRES_DELETE) {
                            continue;
                        }

                        /* test if the node is in subtree to be deleted */
                        for (parent = unres->node[j]; parent; parent = parent->parent) {
                            if (parent == unres->node[i]) {
                                /* yes, it is */
                                unres->type[j] = UNRES_RESOLVED;
                                resolved++;
                                break;
                            }
                        }
                    }
                } else {
                    unres->type[i] = UNRES_RESOLVED;
                }
                if (!ignore_fail) {
                    ly_err_free_next(ctx, prev_eitem);
                }
                resolved++;
                progress = 1;
            } else if (rc == -1) {
                goto error;
            } /* else forward reference */
        }
        first = 0;
    } while (progress && resolved < stmt_count);

    /* do we have some unresolved when-stmt? */
    if (stmt_count > resolved) {
        goto error;
    }

    for (i = 0; del_items && i < unres->count; i++) {
        /* we had some when-stmt resulted to false, so now we have to sanitize the unres list */
        if (unres->type[i] != UNRES_DELETE) {
            continue;
        }
        if (!unres->node[i]) {
            unres->type[i] = UNRES_RESOLVED;
            del_items--;
            continue;
        }

        if (unres->store_diff) {
            resolve_unres_data_autodel_diff(unres, i);
        }

        /* really remove the complete subtree */
        lyd_free(unres->node[i]);
        unres->type[i] = UNRES_RESOLVED;
        del_items--;
    }

    /*
     * now leafrefs
     */
    if (options & LYD_OPT_TRUSTED) {
        /* we want to attempt to resolve leafrefs */
        assert(!ignore_fail);
        ignore_fail = 1;

        ly_ilo_restore(ctx, prev_ilo, prev_eitem, 0);
        ly_errno = prev_ly_errno;
    }
    first = 1;
    stmt_count = 0;
    resolved = 0;
    do {
        progress = 0;
        for (i = 0; i < unres->count; i++) {
            if (unres->type[i] != UNRES_LEAFREF) {
                continue;
            }
            if (first) {
                /* count leafref nodes in unres list */
                stmt_count++;
            }

            rc = resolve_unres_data_item(unres->node[i], unres->type[i], ignore_fail, NULL);
            if (!rc) {
                unres->type[i] = UNRES_RESOLVED;
                if (!ignore_fail) {
                    ly_err_free_next(ctx, prev_eitem);
                }
                resolved++;
                progress = 1;
            } else if (rc == -1) {
                goto error;
            } /* else forward reference */
        }
        first = 0;
    } while (progress && resolved < stmt_count);

    /* do we have some unresolved leafrefs? */
    if (stmt_count > resolved) {
        goto error;
    }

    if (!ignore_fail) {
        /* log normally now, throw away irrelevant errors */
        ly_ilo_restore(ctx, prev_ilo, prev_eitem, 0);
        ly_errno = prev_ly_errno;
    }

    /*
     * rest
     */
    for (i = 0; i < unres->count; ++i) {
        if (unres->type[i] == UNRES_RESOLVED) {
            continue;
        }
        assert(!(options & LYD_OPT_TRUSTED) || ((unres->type[i] != UNRES_MUST) && (unres->type[i] != UNRES_MUST_INOUT)));

        rc = resolve_unres_data_item(unres->node[i], unres->type[i], ignore_fail, NULL);
        if (rc) {
            /* since when was already resolved, a forward reference is an error */
            return -1;
        }

        unres->type[i] = UNRES_RESOLVED;
    }

    LOGVRB("All data nodes and constraints resolved.");
    unres->count = 0;
    return EXIT_SUCCESS;

error:
    if (!ignore_fail) {
        /* print all the new errors */
        ly_ilo_restore(ctx, prev_ilo, prev_eitem, 1);
        /* do not restore ly_errno, it was udpated properly */
    }
    return -1;
}