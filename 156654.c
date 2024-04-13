static uint8_t *alloc_dhcp_option(int code, const uint8_t *data, unsigned size)
{
	uint8_t *storage;

	storage = g_try_malloc(size + OPT_DATA);
	if (!storage)
		return NULL;

	storage[OPT_CODE] = code;
	storage[OPT_LEN] = size;
	memcpy(&storage[OPT_DATA], data, size);

	return storage;
}