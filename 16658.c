ipf_reassembled_list_add(struct ovs_list *reassembled_pkt_list,
                         struct reassembled_pkt *rp)
    /* OVS_REQUIRES(ipf_lock) */
{
    ovs_list_push_back(reassembled_pkt_list, &rp->rp_list_node);
}