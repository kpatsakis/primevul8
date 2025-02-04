evbuffer_add(struct evbuffer *buf, const void *data, size_t datlen)
{
	size_t used = buf->misalign + buf->off;
	size_t oldoff = buf->off;

	if (buf->totallen - used < datlen) {
		if (evbuffer_expand(buf, datlen) == -1)
			return (-1);
	}

	memcpy(buf->buffer + buf->off, data, datlen);
	buf->off += datlen;

	if (datlen && buf->cb != NULL)
		(*buf->cb)(buf, oldoff, buf->off, buf->cbarg);

	return (0);
}