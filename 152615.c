mbuf_size(MBuf *mbuf)
{
	return mbuf->data_end - mbuf->data;
}