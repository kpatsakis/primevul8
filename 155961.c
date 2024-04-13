lha_calcsum(unsigned char sum, const void *pp, int offset, size_t size)
{
	unsigned char const *p = (unsigned char const *)pp;

	p += offset;
	for (;size > 0; --size)
		sum += *p++;
	return (sum);
}