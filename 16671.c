ipf_reassemble_v4_frags(struct ipf_list *ipf_list)
    /* OVS_REQUIRES(ipf_lock) */
{
    struct ipf_frag *frag_list = ipf_list->frag_list;
    struct dp_packet *pkt = dp_packet_clone(frag_list[0].pkt);
    dp_packet_set_size(pkt, dp_packet_size(pkt) - dp_packet_l2_pad_size(pkt));
    struct ip_header *l3 = dp_packet_l3(pkt);
    int len = ntohs(l3->ip_tot_len);

    int rest_len = frag_list[ipf_list->last_inuse_idx].end_data_byte -
                   frag_list[1].start_data_byte + 1;

    if (len + rest_len > IPV4_PACKET_MAX_SIZE) {
        ipf_print_reass_packet(
            "Unsupported big reassembled v4 packet; v4 hdr:", l3);
        dp_packet_delete(pkt);
        return NULL;
    }

    dp_packet_prealloc_tailroom(pkt, rest_len);

    for (int i = 1; i <= ipf_list->last_inuse_idx; i++) {
        size_t add_len = frag_list[i].end_data_byte -
                         frag_list[i].start_data_byte + 1;
        const char *l4 = dp_packet_l4(frag_list[i].pkt);
        dp_packet_put(pkt, l4, add_len);
    }

    len += rest_len;
    l3 = dp_packet_l3(pkt);
    ovs_be16 new_ip_frag_off = l3->ip_frag_off & ~htons(IP_MORE_FRAGMENTS);
    if (!dp_packet_hwol_is_ipv4(pkt)) {
        l3->ip_csum = recalc_csum16(l3->ip_csum, l3->ip_frag_off,
                                    new_ip_frag_off);
        l3->ip_csum = recalc_csum16(l3->ip_csum, l3->ip_tot_len, htons(len));
    }
    l3->ip_tot_len = htons(len);
    l3->ip_frag_off = new_ip_frag_off;
    dp_packet_set_l2_pad_size(pkt, 0);

    return pkt;
}