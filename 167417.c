bool net_tx_pkt_has_fragments(struct NetTxPkt *pkt)
{
    return pkt->raw_frags > 0;
}