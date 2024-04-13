static uint8_t *alloc_dhcpv6_option(uint16_t code, uint8_t *option,
				uint16_t len)
{
	uint8_t *storage;

	storage = g_malloc(2 + 2 + len);
	if (!storage)
		return NULL;

	storage[0] = code >> 8;
	storage[1] = code & 0xff;
	storage[2] = len >> 8;
	storage[3] = len & 0xff;
	memcpy(storage + 2 + 2, option, len);

	return storage;
}