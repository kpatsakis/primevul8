static gboolean send_probe_packet(gpointer dhcp_data)
{
	GDHCPClient *dhcp_client;
	guint timeout;

	dhcp_client = dhcp_data;
	/* if requested_ip is not valid, pick a new address*/
	if (dhcp_client->requested_ip == 0) {
		debug(dhcp_client, "pick a new random address");
		dhcp_client->requested_ip = arp_random_ip();
	}

	debug(dhcp_client, "sending IPV4LL probe request");

	if (dhcp_client->retry_times == 1) {
		dhcp_client->state = IPV4LL_PROBE;
		switch_listening_mode(dhcp_client, L_ARP);
	}
	arp_send_packet(dhcp_client->mac_address, 0,
			dhcp_client->requested_ip, dhcp_client->ifindex);

	if (dhcp_client->retry_times < PROBE_NUM) {
		/*add a random timeout in range of PROBE_MIN to PROBE_MAX*/
		timeout = __connman_util_random_delay_ms(PROBE_MAX-PROBE_MIN);
		timeout += PROBE_MIN*1000;
	} else
		timeout = (ANNOUNCE_WAIT * 1000);

	dhcp_client->timeout = g_timeout_add_full(G_PRIORITY_HIGH,
						 timeout,
						 ipv4ll_probe_timeout,
						 dhcp_client,
						 NULL);
	return FALSE;
}