evbuffer_drain(struct evbuffer *buf, size_t len)
{
	size_t oldoff = buf->off;

	if (len >= buf->off) {
		buf->off = 0;
		buf->buffer = buf->orig_buffer;
		buf->misalign = 0;
		goto done;
	}

	buf->buffer += len;
	buf->misalign += len;

	buf->off -= len;

 done:
	/* Tell someone about changes in this buffer */
	if (buf->off != oldoff && buf->cb != NULL)
		(*buf->cb)(buf, oldoff, buf->off, buf->cbarg);

}