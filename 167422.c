bool net_tx_pkt_send_loopback(struct NetTxPkt *pkt, NetClientState *nc)
{
    bool res;

    pkt->is_loopback = true;
    res = net_tx_pkt_send(pkt, nc);
    pkt->is_loopback = false;

    return res;
}