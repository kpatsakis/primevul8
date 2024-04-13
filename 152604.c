mbuf_rewind(MBuf *mbuf)
{
	mbuf->read_pos = mbuf->data;
	return 0;
}