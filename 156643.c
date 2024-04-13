GDHCPClient *g_dhcp_client_ref(GDHCPClient *dhcp_client)
{
	if (!dhcp_client)
		return NULL;

	__sync_fetch_and_add(&dhcp_client->ref_count, 1);

	return dhcp_client;
}