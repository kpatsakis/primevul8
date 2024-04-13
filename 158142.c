lldp_tlv_put_isid(struct dp_packet *p, uint32_t isid)
{
    uint8_t *data = dp_packet_put_uninit(p, 3);
    data[0] = isid >> 16;
    data[1] = isid >> 8;
    data[2] = isid;
}