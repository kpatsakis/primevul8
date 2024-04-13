lldp_tlv_end(struct dp_packet *p, unsigned int start)
{
    ovs_be16 *tlv = dp_packet_at_assert(p, start, 2);
    *tlv |= htons((dp_packet_size(p) - (start + 2)) & 0x1ff);
}