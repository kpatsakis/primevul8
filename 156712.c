int g_dhcpv6_client_set_oro(GDHCPClient *dhcp_client, int args, ...)
{
	va_list va;
	int i, j, len = sizeof(uint16_t) * args;
	uint8_t *values;

	values = g_try_malloc(len);
	if (!values)
		return -ENOMEM;

	va_start(va, args);
	for (i = 0, j = 0; i < args; i++) {
		uint16_t value = va_arg(va, int);
		values[j++] = value >> 8;
		values[j++] = value & 0xff;
	}
	va_end(va);

	g_dhcpv6_client_set_send(dhcp_client, G_DHCPV6_ORO, values, len);

	g_free(values);

	return 0;
}