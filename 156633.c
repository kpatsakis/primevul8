int g_dhcpv6_create_duid(GDHCPDuidType duid_type, int index, int type,
			unsigned char **duid, int *duid_len)
{
	time_t duid_time;

	switch (duid_type) {
	case G_DHCPV6_DUID_LLT:
		*duid_len = 2 + 2 + 4 + ETH_ALEN;
		*duid = g_try_malloc(*duid_len);
		if (!*duid)
			return -ENOMEM;

		(*duid)[0] = 0;
		(*duid)[1] = 1;
		__connman_inet_get_interface_mac_address(index, &(*duid)[2 + 2 + 4]);
		(*duid)[2] = 0;
		(*duid)[3] = type;
		duid_time = time(NULL) - DUID_TIME_EPOCH;
		(*duid)[4] = duid_time >> 24;
		(*duid)[5] = duid_time >> 16;
		(*duid)[6] = duid_time >> 8;
		(*duid)[7] = duid_time & 0xff;
		break;
	case G_DHCPV6_DUID_EN:
		return -EINVAL;
	case G_DHCPV6_DUID_LL:
		*duid_len = 2 + 2 + ETH_ALEN;
		*duid = g_try_malloc(*duid_len);
		if (!*duid)
			return -ENOMEM;

		(*duid)[0] = 0;
		(*duid)[1] = 3;
		__connman_inet_get_interface_mac_address(index, &(*duid)[2 + 2]);
		(*duid)[2] = 0;
		(*duid)[3] = type;
		break;
	}

	return 0;
}