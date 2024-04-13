int g_dhcp_client_decline(GDHCPClient *dhcp_client, uint32_t requested)
{
	struct dhcp_packet packet;

	dhcp_client->state = DECLINED;
	dhcp_client->retry_times = 0;

	debug(dhcp_client, "sending DHCP decline");

	init_packet(dhcp_client, &packet, DHCPDECLINE);

	packet.xid = dhcp_client->xid;
	packet.secs = dhcp_attempt_secs(dhcp_client);

	if (requested)
		dhcp_add_option_uint32(&packet, DHCP_REQUESTED_IP, requested);

	add_send_options(dhcp_client, &packet);

	return dhcp_send_raw_packet(&packet, INADDR_ANY, CLIENT_PORT,
				INADDR_BROADCAST, SERVER_PORT, MAC_BCAST_ADDR,
				dhcp_client->ifindex, true);
}