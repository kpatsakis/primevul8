void net_tx_pkt_uninit(struct NetTxPkt *pkt)
{
    if (pkt) {
        g_free(pkt->vec);
        g_free(pkt->raw);
        g_free(pkt);
    }
}