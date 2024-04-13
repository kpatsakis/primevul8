char *g_dhcp_client_get_netmask(GDHCPClient *dhcp_client)
{
	GList *option = NULL;

	if (dhcp_client->type == G_DHCP_IPV6)
		return NULL;

	switch (dhcp_client->state) {
	case IPV4LL_DEFEND:
	case IPV4LL_MONITOR:
		return g_strdup("255.255.0.0");
	case BOUND:
	case RENEWING:
	case REBINDING:
		option = g_dhcp_client_get_option(dhcp_client, G_DHCP_SUBNET);
		if (option)
			return g_strdup(option->data);
	case INIT_SELECTING:
	case REBOOTING:
	case REQUESTING:
	case RELEASED:
	case DECLINED:
	case IPV4LL_PROBE:
	case IPV4LL_ANNOUNCE:
	case INFORMATION_REQ:
	case SOLICITATION:
	case REQUEST:
	case CONFIRM:
	case RENEW:
	case REBIND:
	case RELEASE:
	case DECLINE:
		break;
	}
	return NULL;
}