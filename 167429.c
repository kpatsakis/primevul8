void net_tx_pkt_setup_vlan_header_ex(struct NetTxPkt *pkt,
    uint16_t vlan, uint16_t vlan_ethtype)
{
    bool is_new;
    assert(pkt);

    eth_setup_vlan_headers_ex(pkt->vec[NET_TX_PKT_L2HDR_FRAG].iov_base,
        vlan, vlan_ethtype, &is_new);

    /* update l2hdrlen */
    if (is_new) {
        pkt->hdr_len += sizeof(struct vlan_header);
        pkt->vec[NET_TX_PKT_L2HDR_FRAG].iov_len +=
            sizeof(struct vlan_header);
    }
}