mbuf_create(int len)
{
	MBuf	   *mbuf;

	if (!len)
		len = 8192;

	mbuf = px_alloc(sizeof *mbuf);
	mbuf->data = px_alloc(len);
	mbuf->buf_end = mbuf->data + len;
	mbuf->data_end = mbuf->data;
	mbuf->read_pos = mbuf->data;

	mbuf->no_write = false;
	mbuf->own_data = true;

	return mbuf;
}