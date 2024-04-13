mbuf_tell(MBuf *mbuf)
{
	return mbuf->read_pos - mbuf->data;
}