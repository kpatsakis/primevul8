mbuf_free(MBuf *mbuf)
{
	if (mbuf->own_data)
	{
		px_memset(mbuf->data, 0, mbuf->buf_end - mbuf->data);
		px_free(mbuf->data);
	}
	px_free(mbuf);
	return 0;
}