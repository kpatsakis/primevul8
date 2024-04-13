static void listener_watch_destroy(gpointer user_data)
{
	GDHCPClient *dhcp_client = user_data;
	g_dhcp_client_unref(dhcp_client);
}