mbuf_avail(MBuf *mbuf)
{
	return mbuf->data_end - mbuf->read_pos;
}