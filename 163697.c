static inline int http_parse_chunk_size(struct http_msg *msg)
{
	const struct buffer *buf = msg->chn->buf;
	const char *ptr = b_ptr(buf, msg->next);
	const char *ptr_old = ptr;
	const char *end = buf->data + buf->size;
	const char *stop = bi_end(buf);
	unsigned int chunk = 0;

	/* The chunk size is in the following form, though we are only
	 * interested in the size and CRLF :
	 *    1*HEXDIGIT *WSP *[ ';' extensions ] CRLF
	 */
	while (1) {
		int c;
		if (ptr == stop)
			return 0;
		c = hex2i(*ptr);
		if (c < 0) /* not a hex digit anymore */
			break;
		if (unlikely(++ptr >= end))
			ptr = buf->data;
		if (chunk & 0xF8000000) /* integer overflow will occur if result >= 2GB */
			goto error;
		chunk = (chunk << 4) + c;
	}

	/* empty size not allowed */
	if (unlikely(ptr == ptr_old))
		goto error;

	while (http_is_spht[(unsigned char)*ptr]) {
		if (++ptr >= end)
			ptr = buf->data;
		if (unlikely(ptr == stop))
			return 0;
	}

	/* Up to there, we know that at least one byte is present at *ptr. Check
	 * for the end of chunk size.
	 */
	while (1) {
		if (likely(HTTP_IS_CRLF(*ptr))) {
			/* we now have a CR or an LF at ptr */
			if (likely(*ptr == '\r')) {
				if (++ptr >= end)
					ptr = buf->data;
				if (ptr == stop)
					return 0;
			}

			if (*ptr != '\n')
				goto error;
			if (++ptr >= end)
				ptr = buf->data;
			/* done */
			break;
		}
		else if (*ptr == ';') {
			/* chunk extension, ends at next CRLF */
			if (++ptr >= end)
				ptr = buf->data;
			if (ptr == stop)
				return 0;

			while (!HTTP_IS_CRLF(*ptr)) {
				if (++ptr >= end)
					ptr = buf->data;
				if (ptr == stop)
					return 0;
			}
			/* we have a CRLF now, loop above */
			continue;
		}
		else
			goto error;
	}

	/* OK we found our CRLF and now <ptr> points to the next byte,
	 * which may or may not be present. We save that into ->next and
	 * ->sov.
	 */
	if (unlikely(ptr < ptr_old))
		msg->sov += buf->size;
	msg->sov += ptr - ptr_old;
	msg->next = buffer_count(buf, buf->p, ptr);
	msg->chunk_len = chunk;
	msg->body_len += chunk;
	msg->msg_state = chunk ? HTTP_MSG_DATA : HTTP_MSG_TRAILERS;
	return 1;
 error:
	msg->err_pos = buffer_count(buf, buf->p, ptr);
	return -1;
}