static GList *add_prefix(GDHCPClient *dhcp_client, GList *list,
			struct in6_addr *addr,
			unsigned char prefixlen, uint32_t preferred,
			uint32_t valid)
{
	GDHCPIAPrefix *ia_prefix;

	ia_prefix = g_try_new(GDHCPIAPrefix, 1);
	if (!ia_prefix)
		return list;

	if (dhcp_client->debug_func) {
		char addr_str[INET6_ADDRSTRLEN + 1];
		inet_ntop(AF_INET6, addr, addr_str, INET6_ADDRSTRLEN);
		debug(dhcp_client, "prefix %s/%d preferred %u valid %u",
			addr_str, prefixlen, preferred, valid);
	}

	memcpy(&ia_prefix->prefix, addr, sizeof(struct in6_addr));
	ia_prefix->prefixlen = prefixlen;
	ia_prefix->preferred = preferred;
	ia_prefix->valid = valid;
	ia_prefix->expire = time(NULL) + valid;

	return g_list_prepend(list, ia_prefix);
}