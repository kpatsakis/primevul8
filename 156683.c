static void start_bound(GDHCPClient *dhcp_client)
{
	debug(dhcp_client, "start bound");

	dhcp_client->state = BOUND;

	remove_timeouts(dhcp_client);

	/*
	 *TODO: T1 and T2 should be set through options instead of
	 * defaults as they are here.
	 */

	dhcp_client->T1 = dhcp_client->lease_seconds >> 1;
	dhcp_client->T2 = dhcp_client->lease_seconds * 0.875;
	dhcp_client->expire = dhcp_client->lease_seconds;

	dhcp_client->t1_timeout = g_timeout_add_seconds_full(G_PRIORITY_HIGH,
					dhcp_client->T1,
					start_renew, dhcp_client,
							NULL);

	dhcp_client->t2_timeout = g_timeout_add_seconds_full(G_PRIORITY_HIGH,
					dhcp_client->T2,
					start_rebound, dhcp_client,
							NULL);

	dhcp_client->lease_timeout= g_timeout_add_seconds_full(G_PRIORITY_HIGH,
					dhcp_client->expire,
					start_expire, dhcp_client,
							NULL);
}