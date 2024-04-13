pushf_create_mbuf_writer(PushFilter **res, MBuf *dst)
{
	return pushf_create(res, &mbuf_filter, dst, NULL);
}