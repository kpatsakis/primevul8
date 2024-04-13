ipf_expiry_list_clean(struct hmap *frag_lists,
                      struct ipf_list *ipf_list)
    /* OVS_REQUIRES(ipf_lock) */
{
    ipf_list_clean(frag_lists, ipf_list);
}