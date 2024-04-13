static void restart_dhcp(GDHCPClient *dhcp_client, int retry_times)
{
	debug(dhcp_client, "restart DHCP (retries %d)", retry_times);

	remove_timeouts(dhcp_client);

	dhcp_client->retry_times = retry_times;
	dhcp_client->requested_ip = 0;
	dhcp_client->state = INIT_SELECTING;
	switch_listening_mode(dhcp_client, L2);

	g_dhcp_client_start(dhcp_client, dhcp_client->last_address);
}