static inline int http_skip_chunk_crlf(struct http_msg *msg)
{
	const struct buffer *buf = msg->chn->buf;
	const char *ptr;
	int bytes;

	/* NB: we'll check data availabilty at the end. It's not a
	 * problem because whatever we match first will be checked
	 * against the correct length.
	 */
	bytes = 1;
	ptr = buf->p;
	if (*ptr == '\r') {
		bytes++;
		ptr++;
		if (ptr >= buf->data + buf->size)
			ptr = buf->data;
	}

	if (bytes > buf->i)
		return 0;

	if (*ptr != '\n') {
		msg->err_pos = buffer_count(buf, buf->p, ptr);
		return -1;
	}

	ptr++;
	if (unlikely(ptr >= buf->data + buf->size))
		ptr = buf->data;
	/* prepare the CRLF to be forwarded (between ->sol and ->sov) */
	msg->sol = 0;
	msg->sov = msg->next = bytes;
	msg->msg_state = HTTP_MSG_CHUNK_SIZE;
	return 1;
}