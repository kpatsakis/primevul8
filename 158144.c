lldp_tlv_put_u8(struct dp_packet *p, uint8_t x)
{
    dp_packet_put(p, &x, sizeof x);
}