static int send_request(GDHCPClient *dhcp_client)
{
	struct dhcp_packet packet;

	debug(dhcp_client, "sending DHCP request (state %d)",
		dhcp_client->state);

	init_packet(dhcp_client, &packet, DHCPREQUEST);

	packet.xid = dhcp_client->xid;
	packet.secs = dhcp_attempt_secs(dhcp_client);

	if (dhcp_client->state == REQUESTING || dhcp_client->state == REBOOTING)
		dhcp_add_option_uint32(&packet, DHCP_REQUESTED_IP,
				dhcp_client->requested_ip);

	if (dhcp_client->state == REQUESTING)
		dhcp_add_option_uint32(&packet, DHCP_SERVER_ID,
				dhcp_client->server_ip);

	dhcp_add_option_uint16(&packet, DHCP_MAX_SIZE, 576);

	add_request_options(dhcp_client, &packet);

	add_send_options(dhcp_client, &packet);

	if (dhcp_client->state == RENEWING || dhcp_client->state == REBINDING)
		packet.ciaddr = htonl(dhcp_client->requested_ip);

	if (dhcp_client->state == RENEWING)
		return dhcp_send_kernel_packet(&packet,
				dhcp_client->requested_ip, CLIENT_PORT,
				dhcp_client->server_ip, SERVER_PORT,
				dhcp_client->interface);

	return dhcp_send_raw_packet(&packet, INADDR_ANY, CLIENT_PORT,
				INADDR_BROADCAST, SERVER_PORT,
				MAC_BCAST_ADDR, dhcp_client->ifindex,
				dhcp_client->request_bcast);
}