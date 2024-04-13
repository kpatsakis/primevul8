static void ipv4ll_stop(GDHCPClient *dhcp_client)
{

	switch_listening_mode(dhcp_client, L_NONE);

	remove_timeouts(dhcp_client);

	if (dhcp_client->listener_watch > 0) {
		g_source_remove(dhcp_client->listener_watch);
		dhcp_client->listener_watch = 0;
	}

	dhcp_client->state = IPV4LL_PROBE;
	dhcp_client->retry_times = 0;
	dhcp_client->requested_ip = 0;

	g_free(dhcp_client->assigned_ip);
	dhcp_client->assigned_ip = NULL;
}