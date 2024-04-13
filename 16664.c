ipf_is_frag_duped(const struct ipf_frag *frag_list, int last_inuse_idx,
                  size_t start_data_byte, size_t end_data_byte)
    /* OVS_REQUIRES(ipf_lock) */
{
    for (int i = 0; i <= last_inuse_idx; i++) {
        if ((start_data_byte >= frag_list[i].start_data_byte &&
            start_data_byte <= frag_list[i].end_data_byte) ||
            (end_data_byte >= frag_list[i].start_data_byte &&
             end_data_byte <= frag_list[i].end_data_byte)) {
            return true;
        }
    }
    return false;
}