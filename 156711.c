static void add_dhcpv6_binary_option(gpointer key, gpointer value,
					gpointer user_data)
{
	uint8_t *option = value;
	uint16_t len;
	struct hash_params *params = user_data;

	/* option[0][1] contains option code */
	len = option[2] << 8 | option[3];

	if ((*params->ptr_buf + len + 2 + 2) > (params->buf + params->max_buf))
		return;

	memcpy(*params->ptr_buf, option, len + 2 + 2);
	(*params->ptr_buf) += len + 2 + 2;
}