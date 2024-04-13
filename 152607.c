pullf_read_fixed(PullFilter *src, int len, uint8 *dst)
{
	int			res;
	uint8	   *p;

	res = pullf_read_max(src, len, &p, dst);
	if (res < 0)
		return res;
	if (res != len)
	{
		px_debug("pullf_read_fixed: need=%d got=%d", len, res);
		return PXE_MBUF_SHORT_READ;
	}
	if (p != dst)
		memcpy(dst, p, len);
	return 0;
}