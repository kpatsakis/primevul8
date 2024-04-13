void net_tx_pkt_dump(struct NetTxPkt *pkt)
{
#ifdef NET_TX_PKT_DEBUG
    assert(pkt);

    printf("TX PKT: hdr_len: %d, pkt_type: 0x%X, l2hdr_len: %lu, "
        "l3hdr_len: %lu, payload_len: %u\n", pkt->hdr_len, pkt->packet_type,
        pkt->vec[NET_TX_PKT_L2HDR_FRAG].iov_len,
        pkt->vec[NET_TX_PKT_L3HDR_FRAG].iov_len, pkt->payload_len);
#endif
}