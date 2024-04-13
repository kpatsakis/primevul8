void net_tx_pkt_update_ip_checksums(struct NetTxPkt *pkt)
{
    uint16_t csum;
    uint32_t cntr, cso;
    assert(pkt);
    uint8_t gso_type = pkt->virt_hdr.gso_type & ~VIRTIO_NET_HDR_GSO_ECN;
    void *ip_hdr = pkt->vec[NET_TX_PKT_L3HDR_FRAG].iov_base;

    if (pkt->payload_len + pkt->vec[NET_TX_PKT_L3HDR_FRAG].iov_len >
        ETH_MAX_IP_DGRAM_LEN) {
        return;
    }

    if (gso_type == VIRTIO_NET_HDR_GSO_TCPV4 ||
        gso_type == VIRTIO_NET_HDR_GSO_UDP) {
        /* Calculate IP header checksum */
        net_tx_pkt_update_ip_hdr_checksum(pkt);

        /* Calculate IP pseudo header checksum */
        cntr = eth_calc_ip4_pseudo_hdr_csum(ip_hdr, pkt->payload_len, &cso);
        csum = cpu_to_be16(~net_checksum_finish(cntr));
    } else if (gso_type == VIRTIO_NET_HDR_GSO_TCPV6) {
        /* Calculate IP pseudo header checksum */
        cntr = eth_calc_ip6_pseudo_hdr_csum(ip_hdr, pkt->payload_len,
                                            IP_PROTO_TCP, &cso);
        csum = cpu_to_be16(~net_checksum_finish(cntr));
    } else {
        return;
    }

    iov_from_buf(&pkt->vec[NET_TX_PKT_PL_START_FRAG], pkt->payload_frags,
                 pkt->virt_hdr.csum_offset, &csum, sizeof(csum));
}