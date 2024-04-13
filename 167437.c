void net_tx_pkt_reset(struct NetTxPkt *pkt)
{
    int i;

    /* no assert, as reset can be called before tx_pkt_init */
    if (!pkt) {
        return;
    }

    memset(&pkt->virt_hdr, 0, sizeof(pkt->virt_hdr));

    assert(pkt->vec);

    pkt->payload_len = 0;
    pkt->payload_frags = 0;

    assert(pkt->raw);
    for (i = 0; i < pkt->raw_frags; i++) {
        assert(pkt->raw[i].iov_base);
        pci_dma_unmap(pkt->pci_dev, pkt->raw[i].iov_base, pkt->raw[i].iov_len,
                      DMA_DIRECTION_TO_DEVICE, 0);
    }
    pkt->raw_frags = 0;

    pkt->hdr_len = 0;
    pkt->l4proto = 0;
}