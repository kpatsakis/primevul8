resolve_unres_schema_types(struct unres_schema *unres, enum UNRES_ITEM types, struct ly_ctx *ctx, int forward_ref,
                           int print_all_errors, uint32_t *resolved)
{
    uint32_t i, unres_count, res_count;
    int ret = 0, rc;
    struct ly_err_item *prev_eitem;
    enum int_log_opts prev_ilo;
    LY_ERR prev_ly_errno;

    /* if there can be no forward references, every failure is final, so we can print it directly */
    if (forward_ref) {
        prev_ly_errno = ly_errno;
        ly_ilo_change(ctx, ILO_STORE, &prev_ilo, &prev_eitem);
    }

    do {
        unres_count = 0;
        res_count = 0;

        for (i = 0; i < unres->count; ++i) {
            /* UNRES_TYPE_LEAFREF must be resolved (for storing leafref target pointers);
             * if-features are resolved here to make sure that we will have all if-features for
             * later check of feature circular dependency */
            if (unres->type[i] & types) {
                ++unres_count;
                rc = resolve_unres_schema_item(unres->module[i], unres->item[i], unres->type[i], unres->str_snode[i], unres);
                if (unres->type[i] == UNRES_EXT_FINALIZE) {
                    /* to avoid double free */
                    unres->type[i] = UNRES_RESOLVED;
                }
                if (!rc || (unres->type[i] == UNRES_XPATH)) {
                    /* invalid XPath can never cause an error, only a warning */
                    if (unres->type[i] == UNRES_LIST_UNIQ) {
                        /* free the allocated structure */
                        free(unres->item[i]);
                    }

                    unres->type[i] = UNRES_RESOLVED;
                    ++(*resolved);
                    ++res_count;
                } else if ((rc == EXIT_FAILURE) && forward_ref) {
                    /* forward reference, erase errors */
                    ly_err_free_next(ctx, prev_eitem);
                } else if (print_all_errors) {
                    /* just so that we quit the loop */
                    ++res_count;
                    ret = -1;
                } else {
                    if (forward_ref) {
                        ly_ilo_restore(ctx, prev_ilo, prev_eitem, 1);
                    }
                    return -1;
                }
            }
        }
    } while (res_count && (res_count < unres_count));

    if (res_count < unres_count) {
        assert(forward_ref);
        /* just print the errors (but we must free the ones we have and get them again :-/ ) */
        ly_ilo_restore(ctx, prev_ilo, prev_eitem, 0);

        for (i = 0; i < unres->count; ++i) {
            if (unres->type[i] & types) {
                resolve_unres_schema_item(unres->module[i], unres->item[i], unres->type[i], unres->str_snode[i], unres);
            }
        }
        return -1;
    }

    if (forward_ref) {
        /* restore log */
        ly_ilo_restore(ctx, prev_ilo, prev_eitem, 0);
        ly_errno = prev_ly_errno;
    }

    return ret;
}