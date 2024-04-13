save_rfc2307bis_group_memberships(struct sdap_initgr_rfc2307bis_state *state)
{
    errno_t ret, tret;
    int hret;
    TALLOC_CTX *tmp_ctx;
    struct rfc2307bis_group_memberships_state *membership_state;
    struct membership_diff *iter;
    struct membership_diff *iter_start;
    struct membership_diff *iter_tmp;
    bool in_transaction = false;
    int num_added;
    int i;
    int grp_count;
    char **add = NULL;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) return ENOMEM;

    membership_state = talloc_zero(tmp_ctx,
                                struct rfc2307bis_group_memberships_state);
    if (!membership_state) {
        ret = ENOMEM;
        goto done;
    }

    membership_state->sysdb = state->sysdb;
    membership_state->dom = state->dom;
    membership_state->opts = state->opts;
    membership_state->group_hash = state->group_hash;

    hret = hash_iterate(state->group_hash,
                        rfc2307bis_group_memberships_build,
                        membership_state);
    if (hret != HASH_SUCCESS) {
        ret = membership_state->ret;
        goto done;
    }

    ret = sysdb_transaction_start(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to start transaction\n");
        goto done;
    }
    in_transaction = true;

    iter_start = membership_state->memberships;

    DLIST_FOR_EACH(iter, membership_state->memberships) {
        /* Create a copy of iter->add array but do not include groups outside
         * nesting limit. This array must be NULL terminated.
         */
        for (grp_count = 0; iter->add[grp_count]; grp_count++);
        add = talloc_zero_array(tmp_ctx, char *, grp_count + 1);
        if (add == NULL) {
            ret = ENOMEM;
            goto done;
        }

        num_added = 0;
        for (i = 0; i < grp_count; i++) {
            DLIST_FOR_EACH(iter_tmp, iter_start) {
                if (!strcmp(iter_tmp->name,iter->add[i])) {
                    add[num_added] = iter->add[i];
                    num_added++;
                    break;
                }
            }
        }

        if (num_added == 0) {
            add = NULL;
        } else {
            add[num_added] = NULL;
        }
        ret = sysdb_update_members(state->sysdb, state->dom, iter->name,
                                   SYSDB_MEMBER_GROUP,
                                  (const char *const *) add,
                                  (const char *const *) iter->del);
        if (ret != EOK) {
            DEBUG(SSSDBG_MINOR_FAILURE, "Failed to update memberships\n");
            goto done;
        }
    }

    ret = sysdb_transaction_commit(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to commit transaction\n");
        goto done;
    }
    in_transaction = false;

    ret = EOK;
done:
    if (in_transaction) {
        tret = sysdb_transaction_cancel(state->sysdb);
        if (tret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE, "Failed to cancel transaction\n");
        }
    }
    talloc_free(tmp_ctx);
    return ret;
}