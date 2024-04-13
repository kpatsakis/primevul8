mbuf_create_from_data(uint8 *data, int len)
{
	MBuf	   *mbuf;

	mbuf = px_alloc(sizeof *mbuf);
	mbuf->data = (uint8 *) data;
	mbuf->buf_end = mbuf->data + len;
	mbuf->data_end = mbuf->data + len;
	mbuf->read_pos = mbuf->data;

	mbuf->no_write = true;
	mbuf->own_data = false;

	return mbuf;
}