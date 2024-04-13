Controller::resumeRequestBodyChannelWhenAppSinkIdle(Channel *_channel,
	unsigned int size)
{
	FdSinkChannel *channel = reinterpret_cast<FdSinkChannel *>(_channel);
	Request *req = static_cast<Request *>(static_cast<
		ServerKit::BaseHttpRequest *>(channel->getHooks()->userData));
	Client *client = static_cast<Client *>(req->client);
	Controller *self = static_cast<Controller *>(getServerFromClient(client));
	SKC_LOG_EVENT_FROM_STATIC(self, Controller, client, "resumeRequestBodyChannelWhenAppSinkIdle");

	P_ASSERT_EQ(req->state, Request::FORWARDING_BODY_TO_APP);
	req->appSink.setConsumedCallback(NULL);

	if (req->appSink.acceptingInput()) {
		self->startBodyChannel(client, req);
	} else {
		// Either we're done feeding to req->appSink, or req->appSink.feed()
		// encountered an error while writing to the application socket.
		// But we don't care about either scenarios; we just care that
		// ForwardResponse.cpp will now forward the response data and end the
		// request when it's done.
		assert(!req->ended());
		assert(req->appSink.hasError());
		self->logAppSocketWriteError(client, req->appSink.getErrcode());
		req->state = Request::WAITING_FOR_APP_OUTPUT;
	}
}