lldp_tlv_put_u16(struct dp_packet *p, uint16_t x)
{
    ovs_be16 nx = htons(x);
    dp_packet_put(p, &nx, sizeof nx);
}