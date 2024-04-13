static int send_dhcpv6_msg(GDHCPClient *dhcp_client, int type, char *msg)
{
	struct dhcpv6_packet *packet;
	uint8_t buf[MAX_DHCPV6_PKT_SIZE];
	unsigned char *ptr;
	int ret, max_buf;

	memset(buf, 0, sizeof(buf));
	packet = (struct dhcpv6_packet *)&buf[0];
	ptr = buf + sizeof(struct dhcpv6_packet);

	init_packet(dhcp_client, packet, type);

	if (!dhcp_client->retransmit) {
		dhcp_client->xid = packet->transaction_id[0] << 16 |
				packet->transaction_id[1] << 8 |
				packet->transaction_id[2];
		gettimeofday(&dhcp_client->start_time, NULL);
	} else {
		packet->transaction_id[0] = dhcp_client->xid >> 16;
		packet->transaction_id[1] = dhcp_client->xid >> 8 ;
		packet->transaction_id[2] = dhcp_client->xid;
	}

	g_dhcp_client_set_request(dhcp_client, G_DHCPV6_ELAPSED_TIME);

	debug(dhcp_client, "sending DHCPv6 %s message xid 0x%04x", msg,
							dhcp_client->xid);

	max_buf = MAX_DHCPV6_PKT_SIZE - sizeof(struct dhcpv6_packet);

	add_dhcpv6_request_options(dhcp_client, packet, buf, max_buf, &ptr);

	add_dhcpv6_send_options(dhcp_client, buf, max_buf, &ptr);

	ret = dhcpv6_send_packet(dhcp_client->ifindex, packet, ptr - buf);

	debug(dhcp_client, "sent %d pkt %p len %d", ret, packet, ptr - buf);
	return ret;
}