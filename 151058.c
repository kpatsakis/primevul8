Controller::sendBodyToAppWhenAppSinkIdle(Channel *_channel, unsigned int size) {
	FdSinkChannel *channel = reinterpret_cast<FdSinkChannel *>(_channel);
	Request *req = static_cast<Request *>(static_cast<
		ServerKit::BaseHttpRequest *>(channel->getHooks()->userData));
	Client *client = static_cast<Client *>(req->client);
	Controller *self = static_cast<Controller *>(
		getServerFromClient(client));
	SKC_LOG_EVENT_FROM_STATIC(self, Controller, client, "sendBodyToAppWhenAppSinkIdle");

	channel->setConsumedCallback(NULL);
	if (channel->acceptingInput()) {
		self->sendBodyToApp(client, req);
		if (!req->ended()) {
			req->appSource.startReading();
		}
	} else {
		// req->appSink.feed() encountered an error while writing to the
		// application socket. But we don't care about that; we just care that
		// ForwardResponse.cpp will now forward the response data and end the
		// request when it's done.
		UPDATE_TRACE_POINT();
		assert(!req->appSink.ended());
		assert(req->appSink.hasError());
		self->logAppSocketWriteError(client, req->appSink.getErrcode());
		req->state = Request::WAITING_FOR_APP_OUTPUT;
		req->appSource.startReading();
	}
}