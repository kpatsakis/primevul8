ipf_v6_key_extract(struct dp_packet *pkt, ovs_be16 dl_type, uint16_t zone,
                   struct ipf_list_key *key, uint16_t *start_data_byte,
                   uint16_t *end_data_byte, bool *ff, bool *lf)
{
    const struct ovs_16aligned_ip6_hdr *l3 = dp_packet_l3(pkt);
    uint8_t nw_frag = 0;
    uint8_t nw_proto = l3->ip6_nxt;
    const void *data = l3 + 1;
    size_t datasize = dp_packet_l3_size(pkt) - sizeof *l3;
    const struct ovs_16aligned_ip6_frag *frag_hdr = NULL;

    parse_ipv6_ext_hdrs(&data, &datasize, &nw_proto, &nw_frag, &frag_hdr);
    ovs_assert(nw_frag && frag_hdr);
    ovs_be16 ip6f_offlg = frag_hdr->ip6f_offlg;
    *start_data_byte = ntohs(ip6f_offlg & IP6F_OFF_MASK) +
        sizeof (struct ovs_16aligned_ip6_frag);
    *end_data_byte = *start_data_byte + dp_packet_l4_size(pkt) - 1;
    *ff = ipf_is_first_v6_frag(ip6f_offlg);
    *lf = ipf_is_last_v6_frag(ip6f_offlg);
    memset(key, 0, sizeof *key);
    key->ip_id = get_16aligned_be32(&frag_hdr->ip6f_ident);
    key->dl_type = dl_type;
    memcpy(&key->src_addr.ipv6, &l3->ip6_src, sizeof key->src_addr.ipv6);
    /* We are not supporting parsing of the routing header to use as the
     * dst address part of the key. */
    memcpy(&key->dst_addr.ipv6, &l3->ip6_dst, sizeof key->dst_addr.ipv6);
    key->nw_proto = 0;   /* Not used for key for V6. */
    key->zone = zone;
    key->recirc_id = pkt->md.recirc_id;
}