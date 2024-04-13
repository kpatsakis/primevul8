lldp_tlv_start(struct dp_packet *p, uint8_t tlv, unsigned int *start)
{
    *start = dp_packet_size(p);
    lldp_tlv_put_u16(p, tlv << 9);
}