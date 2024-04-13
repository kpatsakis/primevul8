static inline size_t php_mb2_int_to_char(unsigned char *buf, unsigned k)
{
	assert(k <= 0xFFFFU);
	/* one or two bytes */
	if (k <= 0xFFU) { /* 1 */
		buf[0] = k;
		return 1U;
	} else { /* 2 */
		buf[0] = k >> 8;
		buf[1] = k & 0xFFU;
		return 2U;
	}
}