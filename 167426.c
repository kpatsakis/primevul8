void net_tx_pkt_update_ip_hdr_checksum(struct NetTxPkt *pkt)
{
    uint16_t csum;
    assert(pkt);
    struct ip_header *ip_hdr;
    ip_hdr = pkt->vec[NET_TX_PKT_L3HDR_FRAG].iov_base;

    ip_hdr->ip_len = cpu_to_be16(pkt->payload_len +
        pkt->vec[NET_TX_PKT_L3HDR_FRAG].iov_len);

    ip_hdr->ip_sum = 0;
    csum = net_raw_checksum((uint8_t *)ip_hdr,
        pkt->vec[NET_TX_PKT_L3HDR_FRAG].iov_len);
    ip_hdr->ip_sum = cpu_to_be16(csum);
}