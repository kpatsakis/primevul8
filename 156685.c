void g_dhcp_client_clear_values(GDHCPClient *dhcp_client)
{
	g_hash_table_remove_all(dhcp_client->send_value_hash);
}