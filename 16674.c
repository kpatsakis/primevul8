ipf_v4_key_extract(struct dp_packet *pkt, ovs_be16 dl_type, uint16_t zone,
                   struct ipf_list_key *key, uint16_t *start_data_byte,
                   uint16_t *end_data_byte, bool *ff, bool *lf)
{
    const struct ip_header *l3 = dp_packet_l3(pkt);
    uint16_t ip_tot_len = ntohs(l3->ip_tot_len);
    size_t ip_hdr_len = IP_IHL(l3->ip_ihl_ver) * 4;

    *start_data_byte = ntohs(l3->ip_frag_off & htons(IP_FRAG_OFF_MASK)) * 8;
    *end_data_byte = *start_data_byte + ip_tot_len - ip_hdr_len - 1;
    *ff = ipf_is_first_v4_frag(pkt);
    *lf = ipf_is_last_v4_frag(pkt);
    memset(key, 0, sizeof *key);
    key->ip_id = be16_to_be32(l3->ip_id);
    key->dl_type = dl_type;
    key->src_addr.ipv4 = get_16aligned_be32(&l3->ip_src);
    key->dst_addr.ipv4 = get_16aligned_be32(&l3->ip_dst);
    key->nw_proto = l3->ip_proto;
    key->zone = zone;
    key->recirc_id = pkt->md.recirc_id;
    return true;
}