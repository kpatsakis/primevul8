ipf_reassemble_v6_frags(struct ipf_list *ipf_list)
    /* OVS_REQUIRES(ipf_lock) */
{
    struct ipf_frag *frag_list = ipf_list->frag_list;
    struct dp_packet *pkt = dp_packet_clone(frag_list[0].pkt);
    dp_packet_set_size(pkt, dp_packet_size(pkt) - dp_packet_l2_pad_size(pkt));
    struct  ovs_16aligned_ip6_hdr *l3 = dp_packet_l3(pkt);
    int pl = ntohs(l3->ip6_plen) - sizeof(struct ovs_16aligned_ip6_frag);

    int rest_len = frag_list[ipf_list->last_inuse_idx].end_data_byte -
                   frag_list[1].start_data_byte + 1;

    if (pl + rest_len > IPV6_PACKET_MAX_DATA) {
        ipf_print_reass_packet(
             "Unsupported big reassembled v6 packet; v6 hdr:", l3);
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

    pl += rest_len;
    l3 = dp_packet_l3(pkt);

    uint8_t nw_proto = l3->ip6_nxt;
    uint8_t nw_frag = 0;
    const void *data = l3 + 1;
    size_t datasize = pl;

    const struct ovs_16aligned_ip6_frag *frag_hdr = NULL;
    if (!parse_ipv6_ext_hdrs(&data, &datasize, &nw_proto, &nw_frag, &frag_hdr)
        || !nw_frag || !frag_hdr) {

        ipf_print_reass_packet("Unparsed reassembled v6 packet; v6 hdr:", l3);
        dp_packet_delete(pkt);
        return NULL;
    }

    struct ovs_16aligned_ip6_frag *fh =
        CONST_CAST(struct ovs_16aligned_ip6_frag *, frag_hdr);
    fh->ip6f_offlg = 0;
    l3->ip6_plen = htons(pl);
    l3->ip6_ctlun.ip6_un1.ip6_un1_nxt = nw_proto;
    dp_packet_set_l2_pad_size(pkt, 0);
    return pkt;
}