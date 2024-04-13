int g_dhcpv6_client_set_duid(GDHCPClient *dhcp_client, unsigned char *duid,
			int duid_len)
{
	if (!dhcp_client || dhcp_client->type != G_DHCP_IPV6)
		return -EINVAL;

	g_free(dhcp_client->duid);

	dhcp_client->duid = duid;
	dhcp_client->duid_len = duid_len;

	if (dhcp_client->debug_func) {
		gchar *hex = convert_to_hex(duid, duid_len);
		debug(dhcp_client, "DUID(%d) %s", duid_len, hex);
		g_free(hex);
	}

	return 0;
}