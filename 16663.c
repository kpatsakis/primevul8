ipf_list_clean(struct hmap *frag_lists,
               struct ipf_list *ipf_list)
    /* OVS_REQUIRES(ipf_lock) */
{
    ovs_list_remove(&ipf_list->list_node);
    hmap_remove(frag_lists, &ipf_list->node);
    free(ipf_list->frag_list);
    free(ipf_list);
}