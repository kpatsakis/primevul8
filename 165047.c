inline uint32_t _af_byteswap_int32 (uint32_t x)
{
	return ((x & 0x000000ffU) << 24) |
		((x & 0x0000ff00U) << 8) |
		((x & 0x00ff0000U) >> 8) |
		((x & 0xff000000U) >> 24);
}