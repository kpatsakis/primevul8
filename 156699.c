static void ipv4ll_start(GDHCPClient *dhcp_client)
{
	guint timeout;

	remove_timeouts(dhcp_client);

	switch_listening_mode(dhcp_client, L_NONE);
	dhcp_client->retry_times = 0;
	dhcp_client->requested_ip = 0;

	dhcp_client->requested_ip = arp_random_ip();

	/*first wait a random delay to avoid storm of arp request on boot*/
	timeout = __connman_util_random_delay_ms(PROBE_WAIT);

	dhcp_client->retry_times++;
	dhcp_client->timeout = g_timeout_add_full(G_PRIORITY_HIGH,
						timeout,
						send_probe_packet,
						dhcp_client,
						NULL);
}