save_rfc2307bis_groups(struct sdap_initgr_rfc2307bis_state *state)
{
    struct sysdb_attrs **groups = NULL;
    unsigned long count;
    hash_value_t *values;
    int hret, i;
    errno_t ret;
    TALLOC_CTX *tmp_ctx;
    struct sdap_nested_group *gr;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) return ENOMEM;

    hret = hash_values(state->group_hash, &count, &values);
    if (hret != HASH_SUCCESS) {
        ret = EIO;
        goto done;
    }

    groups = talloc_array(tmp_ctx, struct sysdb_attrs *, count);
    if (!groups) {
        ret = ENOMEM;
        goto done;
    }

    for (i = 0; i < count; i++) {
        gr = talloc_get_type(values[i].ptr,
                             struct sdap_nested_group);
        groups[i] = gr->group;
    }
    talloc_zfree(values);

    ret = sdap_nested_groups_store(state->sysdb, state->dom, state->opts,
                                   groups, count);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE, "Could not save groups [%d]: %s\n",
                  ret, strerror(ret));
        goto done;
    }

    ret = EOK;
done:
    talloc_free(tmp_ctx);
    return ret;
}