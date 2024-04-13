static inline uint32_t get_uint32(unsigned char *value)
{
	return value[0] << 24 | value[1] << 16 |
		value[2] << 8 | value[3];
}