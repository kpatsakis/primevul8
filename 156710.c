static uint8_t *alloc_dhcp_string_option(int code, const char *str)
{
	return alloc_dhcp_data_option(code, (const uint8_t *)str, strlen(str));
}