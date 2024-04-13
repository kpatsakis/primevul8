ipf_is_first_v4_frag(const struct dp_packet *pkt)
{
    const struct ip_header *l3 = dp_packet_l3(pkt);
    if (!(l3->ip_frag_off & htons(IP_FRAG_OFF_MASK)) &&
        l3->ip_frag_off & htons(IP_MORE_FRAGMENTS)) {
        return true;
    }
    return false;
}