Controller::stopBodyChannel(Client *client, Request *req) {
	if (req->requestBodyBuffering) {
		req->bodyBuffer.stop();
	} else {
		req->bodyChannel.stop();
	}
}