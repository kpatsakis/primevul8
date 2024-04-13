sdap_nested_groups_store(struct sysdb_ctx *sysdb,
                         struct sss_domain_info *domain,
                         struct sdap_options *opts,
                         struct sysdb_attrs **groups,
                         unsigned long count)
{
    errno_t ret, tret;
    TALLOC_CTX *tmp_ctx;
    char **groupnamelist = NULL;
    bool in_transaction = false;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) return ENOMEM;

    if (count > 0) {
        ret = sysdb_attrs_primary_name_list(sysdb, tmp_ctx,
                                            groups, count,
                                            opts->group_map[SDAP_AT_GROUP_NAME].name,
                                            &groupnamelist);
        if (ret != EOK) {
            DEBUG(SSSDBG_MINOR_FAILURE,
                  "sysdb_attrs_primary_name_list failed [%d]: %s\n",
                    ret, strerror(ret));
            goto done;
        }
    }

    ret = sysdb_transaction_start(sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to start transaction\n");
        goto done;
    }
    in_transaction = true;

    ret = sdap_add_incomplete_groups(sysdb, domain, opts, groupnamelist,
                                     groups, count);
    if (ret != EOK) {
        DEBUG(SSSDBG_TRACE_FUNC, "Could not add incomplete groups [%d]: %s\n",
                   ret, strerror(ret));
        goto done;
    }

    ret = sysdb_transaction_commit(sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to commit transaction\n");
        goto done;
    }
    in_transaction = false;

    ret = EOK;
done:
    if (in_transaction) {
        tret = sysdb_transaction_cancel(sysdb);
        if (tret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE, "Failed to cancel transaction\n");
        }
    }

    talloc_free(tmp_ctx);
    return ret;
}