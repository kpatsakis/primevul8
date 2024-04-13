mbuf_append(MBuf *dst, const uint8 *buf, int len)
{
	if (dst->no_write)
	{
		px_debug("mbuf_append: no_write");
		return PXE_BUG;
	}

	prepare_room(dst, len);

	memcpy(dst->data_end, buf, len);
	dst->data_end += len;

	return 0;
}