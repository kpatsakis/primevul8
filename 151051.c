Controller::sendHeaderToAppWithHttpProtocolWithBuffering(Request *req,
	unsigned int offset, HttpHeaderConstructionCache &cache)
{
	struct iovec *buffers;
	unsigned int nbuffers, dataSize;
	bool ok;

	ok = constructHeaderBuffersForHttpProtocol(req, NULL, 0, nbuffers,
		dataSize, cache);
	assert(ok);

	buffers = (struct iovec *) psg_palloc(req->pool,
		sizeof(struct iovec) * nbuffers);
	ok = constructHeaderBuffersForHttpProtocol(req, buffers, nbuffers,
		nbuffers, dataSize, cache);
	assert(ok);
	(void) ok; // Shut up compiler warning

	MemoryKit::mbuf_pool &mbuf_pool = getContext()->mbuf_pool;
	const unsigned int MBUF_MAX_SIZE = mbuf_pool_data_size(&mbuf_pool);
	if (dataSize <= MBUF_MAX_SIZE) {
		MemoryKit::mbuf buffer(MemoryKit::mbuf_get(&mbuf_pool));
		gatherBuffers(buffer.start, MBUF_MAX_SIZE, buffers, nbuffers);
		buffer = MemoryKit::mbuf(buffer, offset, dataSize - offset);
		req->appSink.feedWithoutRefGuard(boost::move(buffer));
	} else {
		char *buffer = (char *) psg_pnalloc(req->pool, dataSize);
		gatherBuffers(buffer, dataSize, buffers, nbuffers);
		req->appSink.feedWithoutRefGuard(MemoryKit::mbuf(
			buffer + offset, dataSize - offset));
	}
}