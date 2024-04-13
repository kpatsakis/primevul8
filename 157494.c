rfc2307bis_group_memberships_build(hash_entry_t *item, void *user_data)
{
    struct rfc2307bis_group_memberships_state *mstate = talloc_get_type(
                        user_data, struct rfc2307bis_group_memberships_state);
    struct sdap_nested_group *group;
    char *group_name;
    TALLOC_CTX *tmp_ctx;
    errno_t ret;
    char **sysdb_parents_names_list;
    char **ldap_parents_names_list = NULL;

    struct membership_diff *mdiff;

    group_name = (char *) item->key.str;
    group = (struct sdap_nested_group *) item->value.ptr;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) {
        ret = ENOMEM;
        goto done;
    }

    ret = sysdb_get_direct_parents(tmp_ctx, mstate->sysdb, mstate->dom,
                                   SYSDB_MEMBER_GROUP,
                                   group_name, &sysdb_parents_names_list);
    if (ret) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Could not get direct sysdb parents for %s: %d [%s]\n",
                  group_name, ret, strerror(ret));
        goto done;
    }

    if (group->parents_count > 0) {
        ret = sysdb_attrs_primary_name_list(mstate->sysdb, tmp_ctx,
                            group->ldap_parents, group->parents_count,
                            mstate->opts->group_map[SDAP_AT_GROUP_NAME].name,
                            &ldap_parents_names_list);
        if (ret != EOK) {
            goto done;
        }
    }

    ret = build_membership_diff(tmp_ctx, group_name, ldap_parents_names_list,
                                sysdb_parents_names_list, &mdiff);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "Could not build membership diff for %s [%d]: %s\n",
                  group_name, ret, strerror(ret));
        goto done;
    }

    talloc_steal(mstate, mdiff);
    DLIST_ADD(mstate->memberships, mdiff);
    ret = EOK;
done:
    talloc_free(tmp_ctx);
    mstate->ret = ret;
    return ret == EOK ? true : false;
}