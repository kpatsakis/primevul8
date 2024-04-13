static gboolean request_timeout(gpointer user_data)
{
	GDHCPClient *dhcp_client = user_data;

	debug(dhcp_client, "request timeout (retries %d)",
					dhcp_client->retry_times);

	dhcp_client->retry_times++;

	start_request(dhcp_client);

	return FALSE;
}