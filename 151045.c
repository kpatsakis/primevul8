Controller::sendHeaderToAppWithHttpProtocolAndWritev(Request *req, ssize_t &bytesWritten,
	HttpHeaderConstructionCache &cache)
{
	unsigned int maxbuffers = std::min<unsigned int>(
		5 + req->headers.size() * 4 + 4, IOV_MAX);
	struct iovec *buffers = (struct iovec *) psg_palloc(req->pool,
		sizeof(struct iovec) * maxbuffers);
	unsigned int nbuffers, dataSize;

	if (constructHeaderBuffersForHttpProtocol(req, buffers,
		maxbuffers, nbuffers, dataSize, cache))
	{
		ssize_t ret;
		do {
			ret = writev(req->session->fd(), buffers, nbuffers);
		} while (ret == -1 && errno == EINTR);
		bytesWritten = ret;
		return ret == (ssize_t) dataSize;
	} else {
		bytesWritten = 0;
		return false;
	}
}