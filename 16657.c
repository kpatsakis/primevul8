ipf_is_valid_v4_frag(struct ipf *ipf, struct dp_packet *pkt)
{
    if (OVS_UNLIKELY(dp_packet_ip_checksum_bad(pkt))) {
        COVERAGE_INC(ipf_l3csum_err);
        goto invalid_pkt;
    }

    const struct eth_header *l2 = dp_packet_eth(pkt);
    const struct ip_header *l3 = dp_packet_l3(pkt);

    if (OVS_UNLIKELY(!l2 || !l3)) {
        goto invalid_pkt;
    }

    size_t l3_size = dp_packet_l3_size(pkt);
    if (OVS_UNLIKELY(l3_size < IP_HEADER_LEN)) {
        goto invalid_pkt;
    }

    if (!IP_IS_FRAGMENT(l3->ip_frag_off)) {
        return false;
    }

    uint16_t ip_tot_len = ntohs(l3->ip_tot_len);
    if (OVS_UNLIKELY(ip_tot_len != l3_size)) {
        goto invalid_pkt;
    }

    size_t ip_hdr_len = IP_IHL(l3->ip_ihl_ver) * 4;
    if (OVS_UNLIKELY(ip_hdr_len < IP_HEADER_LEN)) {
        goto invalid_pkt;
    }
    if (OVS_UNLIKELY(l3_size < ip_hdr_len)) {
        goto invalid_pkt;
    }

    if (OVS_UNLIKELY(!dp_packet_ip_checksum_valid(pkt)
                     && !dp_packet_hwol_is_ipv4(pkt)
                     && csum(l3, ip_hdr_len) != 0)) {
        COVERAGE_INC(ipf_l3csum_err);
        goto invalid_pkt;
    }

    uint32_t min_v4_frag_size_;
    atomic_read_relaxed(&ipf->min_v4_frag_size, &min_v4_frag_size_);
    bool lf = ipf_is_last_v4_frag(pkt);
    if (OVS_UNLIKELY(!lf && dp_packet_l3_size(pkt) < min_v4_frag_size_)) {
        ipf_count(ipf, false, IPF_NFRAGS_TOO_SMALL);
        goto invalid_pkt;
    }
    return true;

invalid_pkt:
    pkt->md.ct_state = CS_INVALID;
    return false;
}