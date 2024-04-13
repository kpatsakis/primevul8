static uint8_t *alloc_dhcp_data_option(int code, const uint8_t *data,
					unsigned size)
{
	return alloc_dhcp_option(code, data, MIN(size, 255));
}