static gboolean continue_renew (gpointer user_data)
{
	GDHCPClient *dhcp_client = user_data;
	uint64_t rand;

	switch_listening_mode(dhcp_client, L3);
	send_request(dhcp_client);

	if (dhcp_client->t1_timeout > 0)
		g_source_remove(dhcp_client->t1_timeout);

	dhcp_client->t1_timeout = 0;

	dhcp_client->T1 >>= 1;

	if (dhcp_client->T1 > 60) {
		__connman_util_get_random(&rand);
		dhcp_client->t1_timeout = g_timeout_add_full(G_PRIORITY_HIGH,
				dhcp_client->T1 * 1000 + (rand % 2000) - 1000,
				continue_renew,
				dhcp_client,
				NULL);
	}

	return FALSE;
}