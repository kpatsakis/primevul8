prepare_room(MBuf *mbuf, int block_len)
{
	uint8	   *newbuf;
	unsigned	newlen;

	if (mbuf->data_end + block_len <= mbuf->buf_end)
		return;

	newlen = (mbuf->buf_end - mbuf->data)
		+ ((block_len + STEP + STEP - 1) & -STEP);

	newbuf = px_realloc(mbuf->data, newlen);

	mbuf->buf_end = newbuf + newlen;
	mbuf->data_end = newbuf + (mbuf->data_end - mbuf->data);
	mbuf->read_pos = newbuf + (mbuf->read_pos - mbuf->data);
	mbuf->data = newbuf;

	return;
}