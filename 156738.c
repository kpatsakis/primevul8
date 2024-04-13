static int send_release(GDHCPClient *dhcp_client,
			uint32_t server, uint32_t ciaddr)
{
	struct dhcp_packet packet;
	uint64_t rand;

	debug(dhcp_client, "sending DHCP release request");

	init_packet(dhcp_client, &packet, DHCPRELEASE);
	__connman_util_get_random(&rand);
	packet.xid = rand;
	packet.ciaddr = htonl(ciaddr);

	dhcp_add_option_uint32(&packet, DHCP_SERVER_ID, server);

	return dhcp_send_kernel_packet(&packet, ciaddr, CLIENT_PORT,
						server, SERVER_PORT,
						dhcp_client->interface);
}