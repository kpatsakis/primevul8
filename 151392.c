evbuffer_remove(struct evbuffer *buf, void *data, size_t datlen)
{
	size_t nread = datlen;
	if (nread >= buf->off)
		nread = buf->off;

	memcpy(data, buf->buffer, nread);
	evbuffer_drain(buf, nread);
	
	return (nread);
}