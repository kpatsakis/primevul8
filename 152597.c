mbuf_grab(MBuf *mbuf, int len, uint8 **data_p)
{
	if (len > mbuf_avail(mbuf))
		len = mbuf_avail(mbuf);

	mbuf->no_write = true;

	*data_p = mbuf->read_pos;
	mbuf->read_pos += len;
	return len;
}