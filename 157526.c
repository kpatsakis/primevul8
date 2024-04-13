sdap_initgr_store_group_memberships(struct sdap_initgr_nested_state *state)
{
    errno_t ret;
    int i, tret;
    TALLOC_CTX *tmp_ctx;
    struct membership_diff *miter = NULL;
    struct membership_diff *memberships = NULL;
    bool in_transaction = false;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) return ENOMEM;

    /* Compute the diffs first in order to keep the transaction as small
     * as possible
     */
    for (i=0; i < state->groups_cur; i++) {
        ret = sdap_initgr_nested_get_membership_diff(tmp_ctx, state->sysdb,
                                                     state->opts, state->dom,
                                                     state->groups[i],
                                                     state->groups,
                                                     state->groups_cur,
                                                     &miter);
        if (ret) {
            DEBUG(SSSDBG_MINOR_FAILURE,
                  "Could not compute memberships for group %d [%d]: %s\n",
                      i, ret, strerror(ret));
            goto done;
        }

        DLIST_ADD(memberships, miter);
    }

    ret = sysdb_transaction_start(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to start transaction\n");
        goto done;
    }
    in_transaction = true;

    DLIST_FOR_EACH(miter, memberships) {
        ret = sysdb_update_members(state->sysdb, state->dom, miter->name,
                                   SYSDB_MEMBER_GROUP,
                                   (const char *const *) miter->add,
                                   (const char *const *) miter->del);
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