struct virtio_net_hdr *net_tx_pkt_get_vhdr(struct NetTxPkt *pkt)
{
    assert(pkt);
    return &pkt->virt_hdr;
}