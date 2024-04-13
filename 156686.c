static gboolean ipv4ll_defend_timeout(gpointer dhcp_data)
{
	GDHCPClient *dhcp_client = dhcp_data;

	debug(dhcp_client, "back to MONITOR mode");

	dhcp_client->conflicts = 0;
	dhcp_client->state = IPV4LL_MONITOR;

	return FALSE;
}