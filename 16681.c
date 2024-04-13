ipf_sort(struct ipf_frag *frag_list, size_t last_idx)
    /* OVS_REQUIRES(ipf_lock) */
{
    for (int li = 1; li <= last_idx; li++) {
        struct ipf_frag ipf_frag = frag_list[li];
        int ci = li - 1;
        while (ci >= 0 &&
               frag_list[ci].start_data_byte > ipf_frag.start_data_byte) {
            frag_list[ci + 1] = frag_list[ci];
            ci--;
        }
        frag_list[ci + 1] = ipf_frag;
    }
}