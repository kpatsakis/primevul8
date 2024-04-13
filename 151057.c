Controller::sendHeaderToAppWithHttpProtocol(Client *client, Request *req) {
	ssize_t bytesWritten;
	HttpHeaderConstructionCache cache;

	cache.cached = false;

	if (OXT_UNLIKELY(getLogLevel() >= LVL_DEBUG3)) {
		struct iovec *buffers;
		unsigned int nbuffers, dataSize;
		bool ok;

		ok = constructHeaderBuffersForHttpProtocol(req, NULL, 0,
			nbuffers, dataSize, cache);
		assert(ok);

		buffers = (struct iovec *) psg_palloc(req->pool,
			sizeof(struct iovec) * nbuffers);
		ok = constructHeaderBuffersForHttpProtocol(req, buffers, nbuffers,
			nbuffers, dataSize, cache);
		assert(ok);
		(void) ok; // Shut up compiler warning

		char *buffer = (char *) psg_pnalloc(req->pool, dataSize);
		gatherBuffers(buffer, dataSize, buffers, nbuffers);
		SKC_TRACE(client, 3, "Header data: \"" <<
			cEscapeString(StaticString(buffer, dataSize)) << "\"");
	}

	if (!sendHeaderToAppWithHttpProtocolAndWritev(req, bytesWritten, cache)) {
		if (bytesWritten >= 0 || errno == EAGAIN || errno == EWOULDBLOCK) {
			sendHeaderToAppWithHttpProtocolWithBuffering(req, bytesWritten, cache);
		} else {
			int e = errno;
			P_ASSERT_EQ(bytesWritten, -1);
			disconnectWithAppSocketWriteError(&client, e);
		}
	}
}