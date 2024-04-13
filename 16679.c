ipf_list_complete(const struct ipf_list *ipf_list)
    /* OVS_REQUIRES(ipf_lock) */
{
    for (int i = 1; i <= ipf_list->last_inuse_idx; i++) {
        if (ipf_list->frag_list[i - 1].end_data_byte + 1
            != ipf_list->frag_list[i].start_data_byte) {
            return false;
        }
    }
    return true;
}