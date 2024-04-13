sdap_initgr_store_user_memberships(struct sdap_initgr_nested_state *state)
{
    errno_t ret;
    int tret;
    const char *orig_dn;

    char **sysdb_parent_name_list = NULL;
    char **ldap_parent_name_list = NULL;

    int nparents;
    struct sysdb_attrs **ldap_parentlist;
    struct ldb_message_element *el;
    int i, mi;
    char **add_groups;
    char **del_groups;
    TALLOC_CTX *tmp_ctx;
    bool in_transaction = false;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) {
        ret = ENOMEM;
        goto done;
    }

    /* Get direct LDAP parents */
    ret = sysdb_attrs_get_string(state->user, SYSDB_ORIG_DN, &orig_dn);
    if (ret != EOK) {
        DEBUG(SSSDBG_OP_FAILURE, "The user has no original DN\n");
        goto done;
    }

    ldap_parentlist = talloc_zero_array(tmp_ctx, struct sysdb_attrs *,
                                        state->groups_cur + 1);
    if (!ldap_parentlist) {
        ret = ENOMEM;
        goto done;
    }
    nparents = 0;

    for (i=0; i < state->groups_cur ; i++) {
        ret = sysdb_attrs_get_el(state->groups[i], SYSDB_MEMBER, &el);
        if (ret) {
            DEBUG(SSSDBG_MINOR_FAILURE,
                  "A group with no members during initgroups?\n");
            goto done;
        }

        for (mi = 0; mi < el->num_values; mi++) {
            if (strcasecmp((const char *) el->values[mi].data, orig_dn) != 0) {
                continue;
            }

            ldap_parentlist[nparents] = state->groups[i];
            nparents++;
        }
    }

    DEBUG(SSSDBG_TRACE_LIBS,
          "The user %s is a direct member of %d LDAP groups\n",
              state->username, nparents);

    if (nparents == 0) {
        ldap_parent_name_list = NULL;
    } else {
        ret = sysdb_attrs_primary_name_list(state->sysdb, tmp_ctx,
                                            ldap_parentlist,
                                            nparents,
                                            state->opts->group_map[SDAP_AT_GROUP_NAME].name,
                                            &ldap_parent_name_list);
        if (ret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE,
                  "sysdb_attrs_primary_name_list failed [%d]: %s\n",
                      ret, strerror(ret));
            goto done;
        }
    }

    ret = sysdb_get_direct_parents(tmp_ctx, state->sysdb, state->dom,
                                   SYSDB_MEMBER_USER,
                                   state->username, &sysdb_parent_name_list);
    if (ret) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Could not get direct sysdb parents for %s: %d [%s]\n",
                   state->username, ret, strerror(ret));
        goto done;
    }

    ret = diff_string_lists(tmp_ctx,
                            ldap_parent_name_list, sysdb_parent_name_list,
                            &add_groups, &del_groups, NULL);
    if (ret != EOK) {
        goto done;
    }

    ret = sysdb_transaction_start(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to start transaction\n");
        goto done;
    }
    in_transaction = true;

    DEBUG(SSSDBG_TRACE_INTERNAL,
          "Updating memberships for %s\n", state->username);
    ret = sysdb_update_members(state->sysdb, state->dom,
                               state->username, SYSDB_MEMBER_USER,
                               (const char *const *) add_groups,
                               (const char *const *) del_groups);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Could not update sysdb memberships for %s: %d [%s]\n",
                  state->username, ret, strerror(ret));
        goto done;
    }

    ret = sysdb_transaction_commit(state->sysdb);
    if (ret != EOK) {
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
    talloc_zfree(tmp_ctx);
    return ret;
}