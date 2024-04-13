static int http_forward_trailers(struct http_msg *msg)
{
	const struct buffer *buf = msg->chn->buf;

	/* we have msg->next which points to next line. Look for CRLF. */
	while (1) {
		const char *p1 = NULL, *p2 = NULL;
		const char *ptr = b_ptr(buf, msg->next);
		const char *stop = bi_end(buf);
		int bytes;

		/* scan current line and stop at LF or CRLF */
		while (1) {
			if (ptr == stop)
				return 0;

			if (*ptr == '\n') {
				if (!p1)
					p1 = ptr;
				p2 = ptr;
				break;
			}

			if (*ptr == '\r') {
				if (p1) {
					msg->err_pos = buffer_count(buf, buf->p, ptr);
					return -1;
				}
				p1 = ptr;
			}

			ptr++;
			if (ptr >= buf->data + buf->size)
				ptr = buf->data;
		}

		/* after LF; point to beginning of next line */
		p2++;
		if (p2 >= buf->data + buf->size)
			p2 = buf->data;

		bytes = p2 - b_ptr(buf, msg->next);
		if (bytes < 0)
			bytes += buf->size;

		/* schedule this line for forwarding */
		msg->sov += bytes;
		if (msg->sov >= buf->size)
			msg->sov -= buf->size;

		if (p1 == b_ptr(buf, msg->next)) {
			/* LF/CRLF at beginning of line => end of trailers at p2.
			 * Everything was scheduled for forwarding, there's nothing
			 * left from this message.
			 */
			msg->next = buffer_count(buf, buf->p, p2);
			msg->msg_state = HTTP_MSG_DONE;
			return 1;
		}
		/* OK, next line then */
		msg->next = buffer_count(buf, buf->p, p2);
	}
}