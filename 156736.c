static gboolean start_expire(gpointer user_data)
{
	GDHCPClient *dhcp_client = user_data;

	debug(dhcp_client, "lease expired");

	/*remove all timeouts if they are set*/
	remove_timeouts(dhcp_client);

	restart_dhcp(dhcp_client, 0);

	/* ip need to be cleared */
	if (dhcp_client->lease_lost_cb)
		dhcp_client->lease_lost_cb(dhcp_client,
				dhcp_client->lease_lost_data);

	return false;
}