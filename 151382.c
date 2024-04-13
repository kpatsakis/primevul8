evbuffer_add_buffer(struct evbuffer *outbuf, struct evbuffer *inbuf)
{
	int res;

	/* Short cut for better performance */
	if (outbuf->off == 0) {
		struct evbuffer tmp;
		size_t oldoff = inbuf->off;

		/* Swap them directly */
		SWAP(&tmp, outbuf);
		SWAP(outbuf, inbuf);
		SWAP(inbuf, &tmp);

		/* 
		 * Optimization comes with a price; we need to notify the
		 * buffer if necessary of the changes. oldoff is the amount
		 * of data that we transfered from inbuf to outbuf
		 */
		if (inbuf->off != oldoff && inbuf->cb != NULL)
			(*inbuf->cb)(inbuf, oldoff, inbuf->off, inbuf->cbarg);
		if (oldoff && outbuf->cb != NULL)
			(*outbuf->cb)(outbuf, 0, oldoff, outbuf->cbarg);
		
		return (0);
	}

	res = evbuffer_add(outbuf, inbuf->buffer, inbuf->off);
	if (res == 0) {
		/* We drain the input buffer on success */
		evbuffer_drain(inbuf, inbuf->off);
	}

	return (res);
}