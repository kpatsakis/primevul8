sdap_initgr_nested_get_membership_diff(TALLOC_CTX *mem_ctx,
                                       struct sysdb_ctx *sysdb,
                                       struct sdap_options *opts,
                                       struct sss_domain_info *dom,
                                       struct sysdb_attrs *group,
                                       struct sysdb_attrs **all_groups,
                                       int groups_count,
                                       struct membership_diff **_mdiff)
{
    errno_t ret;
    struct membership_diff *mdiff;
    const char *group_name;

    struct sysdb_attrs **ldap_parentlist;
    int parents_count;

    char **ldap_parent_names_list = NULL;
    char **sysdb_parents_names_list = NULL;

    TALLOC_CTX *tmp_ctx;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) {
        ret = ENOMEM;
        goto done;
    }

    /* Get direct sysdb parents */
    ret = sdap_get_group_primary_name(tmp_ctx, opts, group, dom, &group_name);
    if (ret != EOK) {
        goto done;
    }

    ret = sysdb_get_direct_parents(tmp_ctx, sysdb, dom,
                                   SYSDB_MEMBER_GROUP,
                                   group_name, &sysdb_parents_names_list);
    if (ret) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Could not get direct sysdb parents for %s: %d [%s]\n",
                   group_name, ret, strerror(ret));
        goto done;
    }

    /* For each group, filter only parents from full set */
    ret = sdap_initgr_nested_get_direct_parents(tmp_ctx,
                                                group,
                                                all_groups,
                                                groups_count,
                                                &ldap_parentlist,
                                                &parents_count);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Cannot get parent groups for %s [%d]: %s\n",
                  group_name, ret, strerror(ret));
        goto done;
    }
    DEBUG(SSSDBG_TRACE_LIBS,
          "The group %s is a direct member of %d LDAP groups\n",
               group_name, parents_count);

    if (parents_count > 0) {
        ret = sysdb_attrs_primary_name_list(sysdb, tmp_ctx,
                                            ldap_parentlist,
                                            parents_count,
                                            opts->group_map[SDAP_AT_GROUP_NAME].name,
                                            &ldap_parent_names_list);
        if (ret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE,
                  "sysdb_attrs_primary_name_list failed [%d]: %s\n",
                        ret, strerror(ret));
            goto done;
        }
    }

    ret = build_membership_diff(tmp_ctx, group_name, ldap_parent_names_list,
                                sysdb_parents_names_list, &mdiff);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "Could not build membership diff for %s [%d]: %s\n",
                  group_name, ret, strerror(ret));
        goto done;
    }

    ret = EOK;
    *_mdiff = talloc_steal(mem_ctx, mdiff);
done:
    talloc_free(tmp_ctx);
    return ret;
}