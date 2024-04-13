GList *g_dhcp_client_get_option(GDHCPClient *dhcp_client,
					unsigned char option_code)
{
	return g_hash_table_lookup(dhcp_client->code_value_hash,
					GINT_TO_POINTER((int) option_code));
}