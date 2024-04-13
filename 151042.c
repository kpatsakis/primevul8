Controller::whenSendingRequest_onRequestBody(Client *client, Request *req,
	const MemoryKit::mbuf &buffer, int errcode)
{
	TRACE_POINT();

	if (buffer.size() > 0) {
		// Data
		if (req->bodyType == Request::RBT_CONTENT_LENGTH) {
			SKC_TRACE(client, 3, "Forwarding " << buffer.size() <<
				" bytes of client request body (" << req->bodyAlreadyRead <<
				" of " << req->aux.bodyInfo.contentLength << " bytes forwarded in total): \"" <<
				cEscapeString(StaticString(buffer.start, buffer.size())) <<
				"\"");
		} else {
			SKC_TRACE(client, 3, "Forwarding " << buffer.size() <<
				" bytes of client request body (" << req->bodyAlreadyRead <<
				" bytes forwarded in total): \"" <<
				cEscapeString(StaticString(buffer.start, buffer.size())) <<
				"\"");
		}
		req->appSink.feed(buffer);
		if (!req->appSink.acceptingInput()) {
			if (req->appSink.mayAcceptInputLater()) {
				SKC_TRACE(client, 3, "Waiting for appSink channel to become "
					"idle before continuing sending body to application");
				req->appSink.setConsumedCallback(resumeRequestBodyChannelWhenAppSinkIdle);
				stopBodyChannel(client, req);
				return Channel::Result(buffer.size(), false);
			} else {
				// Either we're done feeding to req->appSink, or req->appSink.feed()
				// encountered an error while writing to the application socket.
				// But we don't care about either scenarios; we just care that
				// ForwardResponse.cpp will now forward the response data and end the
				// request when it's done.
				assert(!req->ended());
				assert(req->appSink.hasError());
				logAppSocketWriteError(client, req->appSink.getErrcode());
				req->state = Request::WAITING_FOR_APP_OUTPUT;
				stopBodyChannel(client, req);
			}
		}
		return Channel::Result(buffer.size(), false);
	} else if (errcode == 0 || errcode == ECONNRESET) {
		// EOF
		SKC_TRACE(client, 2, "End of request body encountered");
		// Our task is done. ForwardResponse.cpp will take
		// care of ending the request, once all response
		// data is forwarded.
		req->state = Request::WAITING_FOR_APP_OUTPUT;
		halfCloseAppSink(client, req);
		return Channel::Result(0, true);
	} else {
		const unsigned int BUFSIZE = 1024;
		char *message = (char *) psg_pnalloc(req->pool, BUFSIZE);
		int size = snprintf(message, BUFSIZE,
			"error reading request body: %s (errno=%d)",
			ServerKit::getErrorDesc(errcode), errcode);
		disconnectWithError(&client, StaticString(message, size));
		return Channel::Result(0, true);
	}
}