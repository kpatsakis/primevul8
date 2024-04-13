void g_dhcp_client_set_debug(GDHCPClient *dhcp_client,
				GDHCPDebugFunc func, gpointer user_data)
{
	if (!dhcp_client)
		return;

	dhcp_client->debug_func = func;
	dhcp_client->debug_data = user_data;
}