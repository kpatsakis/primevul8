lldp_tlv_put_u32(struct dp_packet *p, uint32_t x)
{
    ovs_be32 nx = htonl(x);
    dp_packet_put(p, &nx, sizeof nx);
}