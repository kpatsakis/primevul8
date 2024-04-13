eth_pkt_types_e net_tx_pkt_get_packet_type(struct NetTxPkt *pkt)
{
    assert(pkt);

    return pkt->packet_type;
}