ipf_reassembled_list_remove(struct reassembled_pkt *rp)
    /* OVS_REQUIRES(ipf_lock) */
{
    ovs_list_remove(&rp->rp_list_node);
}