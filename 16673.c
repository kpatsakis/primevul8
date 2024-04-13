ipf_expiry_list_remove(struct ipf_list *ipf_list)
    /* OVS_REQUIRES(ipf_lock) */
{
    ovs_list_remove(&ipf_list->list_node);
}