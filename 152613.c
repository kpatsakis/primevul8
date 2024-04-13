mbuf_steal_data(MBuf *mbuf, uint8 **data_p)
{
	int			len = mbuf_size(mbuf);

	mbuf->no_write = true;
	mbuf->own_data = false;

	*data_p = mbuf->data;

	mbuf->data = mbuf->data_end = mbuf->read_pos = mbuf->buf_end = NULL;

	return len;
}