static gboolean send_announce_packet(gpointer dhcp_data)
{
	GDHCPClient *dhcp_client;

	dhcp_client = dhcp_data;

	debug(dhcp_client, "sending IPV4LL announce request");

	arp_send_packet(dhcp_client->mac_address,
				dhcp_client->requested_ip,
				dhcp_client->requested_ip,
				dhcp_client->ifindex);

	remove_timeouts(dhcp_client);

	if (dhcp_client->state == IPV4LL_DEFEND) {
		dhcp_client->timeout =
			g_timeout_add_seconds_full(G_PRIORITY_HIGH,
						DEFEND_INTERVAL,
						ipv4ll_defend_timeout,
						dhcp_client,
						NULL);
		return TRUE;
	} else
		dhcp_client->timeout =
			g_timeout_add_seconds_full(G_PRIORITY_HIGH,
						ANNOUNCE_INTERVAL,
						ipv4ll_announce_timeout,
						dhcp_client,
						NULL);
	return TRUE;
}