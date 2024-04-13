static void net_tx_pkt_calculate_hdr_len(struct NetTxPkt *pkt)
{
    pkt->hdr_len = pkt->vec[NET_TX_PKT_L2HDR_FRAG].iov_len +
        pkt->vec[NET_TX_PKT_L3HDR_FRAG].iov_len;
}