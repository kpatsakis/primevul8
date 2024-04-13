static void add_dhcpv6_send_options(GDHCPClient *dhcp_client,
				unsigned char *buf, int max_buf,
				unsigned char **ptr_buf)
{
	struct hash_params params = {
		.buf = buf,
		.max_buf = max_buf,
		.ptr_buf = ptr_buf
	};

	if (dhcp_client->type != G_DHCP_IPV6)
		return;

	g_hash_table_foreach(dhcp_client->send_value_hash,
				add_dhcpv6_binary_option, &params);

	*ptr_buf = *params.ptr_buf;
}