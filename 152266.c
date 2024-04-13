cdf_offset(const void *p, size_t l)
{
	return CAST(const void *, CAST(const uint8_t *, p) + l);
}