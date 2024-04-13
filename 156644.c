static gboolean discover_timeout(gpointer user_data)
{
	GDHCPClient *dhcp_client = user_data;

	dhcp_client->retry_times++;

	/*
	 * We do not send the REQUESTED IP option if we are retrying because
	 * if the server is non-authoritative it will ignore the request if the
	 * option is present.
	 */
	g_dhcp_client_start(dhcp_client, NULL);

	return FALSE;
}