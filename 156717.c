static bool check_package_owner(GDHCPClient *dhcp_client, gpointer pkt)
{
	if (dhcp_client->type == G_DHCP_IPV6) {
		struct dhcpv6_packet *packet6 = pkt;
		uint32_t xid;

		if (!packet6)
			return false;

		xid = packet6->transaction_id[0] << 16 |
			packet6->transaction_id[1] << 8 |
			packet6->transaction_id[2];

		if (xid != dhcp_client->xid)
			return false;
	} else {
		struct dhcp_packet *packet = pkt;

		if (packet->xid != dhcp_client->xid)
			return false;

		if (packet->hlen != 6)
			return false;

		if (memcmp(packet->chaddr, dhcp_client->mac_address, 6))
			return false;
	}

	return true;
}