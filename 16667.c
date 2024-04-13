ipf_list_init(struct ipf_list *ipf_list, struct ipf_list_key *key,
              int max_frag_list_size)
{
    ipf_list->key = *key;
    ipf_list->last_inuse_idx = IPF_INVALID_IDX;
    ipf_list->last_sent_idx = IPF_INVALID_IDX;
    ipf_list->reass_execute_ctx = NULL;
    ipf_list->state = IPF_LIST_STATE_UNUSED;
    ipf_list->size = max_frag_list_size;
    ipf_list->frag_list
        = xzalloc(ipf_list->size * sizeof *ipf_list->frag_list);
}