ipf_completed_list_add(struct ovs_list *frag_complete_list,
                       struct ipf_list *ipf_list)
    /* OVS_REQUIRES(ipf_lock) */
{
    ovs_list_push_back(frag_complete_list, &ipf_list->list_node);
}