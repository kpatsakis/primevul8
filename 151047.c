Controller::startBodyChannel(Client *client, Request *req) {
	if (req->requestBodyBuffering) {
		req->bodyBuffer.start();
	} else {
		req->bodyChannel.start();
	}
}