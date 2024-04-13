resolve_json_data_node_hash(struct lyd_node *parent, struct parsed_pred pp)
{
    values_equal_cb prev_cb;
    struct lyd_node **ret = NULL;
    uint32_t hash;
    int i;

    assert(parent && parent->hash);

    /* set our value equivalence callback that does not require data nodes */
    prev_cb = lyht_set_cb(parent->ht, resolve_hash_table_find_equal);

    /* get the hash of the searched node */
    hash = dict_hash_multi(0, lys_node_module(pp.schema)->name, strlen(lys_node_module(pp.schema)->name));
    hash = dict_hash_multi(hash, pp.schema->name, strlen(pp.schema->name));
    if (pp.schema->nodetype == LYS_LEAFLIST) {
        assert((pp.len == 1) && (pp.pred[0].name[0] == '.') && (pp.pred[0].nam_len == 1));
        /* leaf-list value in predicate */
        hash = dict_hash_multi(hash, pp.pred[0].value, pp.pred[0].val_len);
    } else if (pp.schema->nodetype == LYS_LIST) {
        /* list keys in predicates */
        for (i = 0; i < pp.len; ++i) {
            hash = dict_hash_multi(hash, pp.pred[i].value, pp.pred[i].val_len);
        }
    }
    hash = dict_hash_multi(hash, NULL, 0);

    /* try to find the node */
    i = lyht_find(parent->ht, &pp, hash, (void **)&ret);
    assert(i || *ret);

    /* restore the original callback */
    lyht_set_cb(parent->ht, prev_cb);

    return (i ? NULL : *ret);
}