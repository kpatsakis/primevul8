inline uint16_t _af_byteswap_int16 (uint16_t x)
{
	return (x >> 8) | (x << 8);
}