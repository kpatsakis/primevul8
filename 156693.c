void g_dhcp_client_clear_requests(GDHCPClient *dhcp_client)
{
	g_list_free(dhcp_client->request_list);
	dhcp_client->request_list = NULL;
}