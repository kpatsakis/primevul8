build_membership_diff(TALLOC_CTX *mem_ctx, const char *name,
                      char **ldap_parent_names, char **sysdb_parent_names,
                      struct membership_diff **_mdiff)
{
    TALLOC_CTX *tmp_ctx;
    struct membership_diff *mdiff;
    errno_t ret;
    char **add_groups;
    char **del_groups;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) {
        ret = ENOMEM;
        goto done;
    }

    mdiff = talloc_zero(tmp_ctx, struct membership_diff);
    if (!mdiff) {
        ret = ENOMEM;
        goto done;
    }
    mdiff->name = talloc_strdup(mdiff, name);
    if (!mdiff->name) {
        ret = ENOMEM;
        goto done;
    }

    /* Find the differences between the sysdb and ldap lists
     * Groups in ldap only must be added to the sysdb;
     * groups in the sysdb only must be removed.
     */
    ret = diff_string_lists(tmp_ctx,
                            ldap_parent_names, sysdb_parent_names,
                            &add_groups, &del_groups, NULL);
    if (ret != EOK) {
        goto done;
    }
    mdiff->add = talloc_steal(mdiff, add_groups);
    mdiff->del = talloc_steal(mdiff, del_groups);

    ret = EOK;
    *_mdiff = talloc_steal(mem_ctx, mdiff);
done:
    talloc_free(tmp_ctx);
    return ret;
}