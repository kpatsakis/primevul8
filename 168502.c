static unsigned hash_name(const char *name, int namelen)
{
	unsigned val = 0;
	unsigned char c;

	while (namelen--) {
		c = *name++;
		val = ((val << 7) | (val >> 22)) ^ c;
	}
	return val;
}