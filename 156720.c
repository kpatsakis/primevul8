static void remove_timeouts(GDHCPClient *dhcp_client)
{

	if (dhcp_client->timeout > 0)
		g_source_remove(dhcp_client->timeout);
	if (dhcp_client->t1_timeout > 0)
		g_source_remove(dhcp_client->t1_timeout);
	if (dhcp_client->t2_timeout > 0)
		g_source_remove(dhcp_client->t2_timeout);
	if (dhcp_client->lease_timeout > 0)
		g_source_remove(dhcp_client->lease_timeout);

	dhcp_client->timeout = 0;
	dhcp_client->t1_timeout = 0;
	dhcp_client->t2_timeout = 0;
	dhcp_client->lease_timeout = 0;

}