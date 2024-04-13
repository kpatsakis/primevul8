static inline uint16_t get_uint16(unsigned char *value)
{
	return value[0] << 8 | value[1];
}