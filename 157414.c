static bool bnxt_re_is_loopback_packet(struct bnxt_re_dev *rdev,
				       void *rq_hdr_buf)
{
	u8 *tmp_buf = NULL;
	struct ethhdr *eth_hdr;
	u16 eth_type;
	bool rc = false;

	tmp_buf = (u8 *)rq_hdr_buf;
	/*
	 * If dest mac is not same as I/F mac, this could be a
	 * loopback address or multicast address, check whether
	 * it is a loopback packet
	 */
	if (!ether_addr_equal(tmp_buf, rdev->netdev->dev_addr)) {
		tmp_buf += 4;
		/* Check the  ether type */
		eth_hdr = (struct ethhdr *)tmp_buf;
		eth_type = ntohs(eth_hdr->h_proto);
		switch (eth_type) {
		case ETH_P_IBOE:
			rc = true;
			break;
		case ETH_P_IP:
		case ETH_P_IPV6: {
			u32 len;
			struct udphdr *udp_hdr;

			len = (eth_type == ETH_P_IP ? sizeof(struct iphdr) :
						      sizeof(struct ipv6hdr));
			tmp_buf += sizeof(struct ethhdr) + len;
			udp_hdr = (struct udphdr *)tmp_buf;
			if (ntohs(udp_hdr->dest) ==
				    ROCE_V2_UDP_DPORT)
				rc = true;
			break;
			}
		default:
			break;
		}
	}

	return rc;
}