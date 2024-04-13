ipf_is_valid_v6_frag(struct ipf *ipf, struct dp_packet *pkt)
{
    const struct eth_header *l2 = dp_packet_eth(pkt);
    const struct  ovs_16aligned_ip6_hdr *l3 = dp_packet_l3(pkt);
    const char *l4 = dp_packet_l4(pkt);

    if (OVS_UNLIKELY(!l2 || !l3 || !l4)) {
        goto invalid_pkt;
    }

    size_t l3_size = dp_packet_l3_size(pkt);
    size_t l3_hdr_size = sizeof *l3;

    if (OVS_UNLIKELY(l3_size < l3_hdr_size)) {
        goto invalid_pkt;
    }

    uint8_t nw_frag = 0;
    uint8_t nw_proto = l3->ip6_nxt;
    const void *data = l3 + 1;
    size_t datasize = l3_size - l3_hdr_size;
    const struct ovs_16aligned_ip6_frag *frag_hdr = NULL;
    if (!parse_ipv6_ext_hdrs(&data, &datasize, &nw_proto, &nw_frag,
                             &frag_hdr) || !nw_frag || !frag_hdr) {
        return false;
    }

    int pl = ntohs(l3->ip6_plen);
    if (OVS_UNLIKELY(pl + l3_hdr_size != l3_size)) {
        goto invalid_pkt;
    }

    ovs_be16 ip6f_offlg = frag_hdr->ip6f_offlg;
    if (OVS_UNLIKELY(!ipf_is_v6_frag(ip6f_offlg))) {
        return false;
    }

    uint32_t min_v6_frag_size_;
    atomic_read_relaxed(&ipf->min_v6_frag_size, &min_v6_frag_size_);
    bool lf = ipf_is_last_v6_frag(ip6f_offlg);

    if (OVS_UNLIKELY(!lf && dp_packet_l3_size(pkt) < min_v6_frag_size_)) {
        ipf_count(ipf, true, IPF_NFRAGS_TOO_SMALL);
        goto invalid_pkt;
    }

    return true;

invalid_pkt:
    pkt->md.ct_state = CS_INVALID;
    return false;

}