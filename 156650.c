void g_dhcp_client_unref(GDHCPClient *dhcp_client)
{
	if (!dhcp_client)
		return;

	if (__sync_fetch_and_sub(&dhcp_client->ref_count, 1) != 1)
		return;

	g_dhcp_client_stop(dhcp_client);

	g_free(dhcp_client->interface);
	g_free(dhcp_client->assigned_ip);
	g_free(dhcp_client->last_address);
	g_free(dhcp_client->duid);
	g_free(dhcp_client->server_duid);

	g_list_free(dhcp_client->request_list);
	g_list_free(dhcp_client->require_list);

	g_hash_table_destroy(dhcp_client->code_value_hash);
	g_hash_table_destroy(dhcp_client->send_value_hash);
	g_hash_table_destroy(dhcp_client->secs_bcast_hash);

	g_free(dhcp_client);
}