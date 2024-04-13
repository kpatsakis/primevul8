Controller::sendHeaderToAppWithSessionProtocol(Client *client, Request *req) {
	TRACE_POINT();
	SessionProtocolWorkingState state;

	// Workaround for Ruby < 2.1 support.
	std::string delta_monotonic = boost::to_string(SystemTime::getUsec() - (uv_hrtime() / 1000));

	unsigned int bufferSize = determineHeaderSizeForSessionProtocol(req,
		state, delta_monotonic);
	MemoryKit::mbuf_pool &mbuf_pool = getContext()->mbuf_pool;
	const unsigned int MBUF_MAX_SIZE = mbuf_pool_data_size(&mbuf_pool);
	bool ok;

	if (bufferSize <= MBUF_MAX_SIZE) {
		MemoryKit::mbuf buffer(MemoryKit::mbuf_get(&mbuf_pool));
		bufferSize = MBUF_MAX_SIZE;

		ok = constructHeaderForSessionProtocol(req, buffer.start,
			bufferSize, state, delta_monotonic);
		assert(ok);
		buffer = MemoryKit::mbuf(buffer, 0, bufferSize);
		SKC_TRACE(client, 3, "Header data: \"" << cEscapeString(
			StaticString(buffer.start, bufferSize)) << "\"");
		req->appSink.feedWithoutRefGuard(boost::move(buffer));
	} else {
		char *buffer = (char *) psg_pnalloc(req->pool, bufferSize);

		ok = constructHeaderForSessionProtocol(req, buffer,
			bufferSize, state, delta_monotonic);
		assert(ok);
		SKC_TRACE(client, 3, "Header data: \"" << cEscapeString(
			StaticString(buffer, bufferSize)) << "\"");
		req->appSink.feedWithoutRefGuard(MemoryKit::mbuf(
			buffer, bufferSize));
	}

	(void) ok; // Shut up compiler warning
}