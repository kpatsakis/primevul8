size_t net_tx_pkt_get_total_len(struct NetTxPkt *pkt)
{
    assert(pkt);

    return pkt->hdr_len + pkt->payload_len;
}