Controller::sendHeaderToApp(Client *client, Request *req) {
	TRACE_POINT();
	SKC_TRACE(client, 2, "Sending headers to application with " <<
		req->session->getProtocol() << " protocol");
	req->state = Request::SENDING_HEADER_TO_APP;

	/**
	 * HTTP does not formally support half-closing, and Node.js treats a
	 * half-close as a full close, so we only half-close session sockets, not
	 * HTTP sockets.
	 */
	if (req->session->getProtocol() == "session") {
		UPDATE_TRACE_POINT();
		req->halfCloseAppConnection = req->bodyType != Request::RBT_NO_BODY;
		sendHeaderToAppWithSessionProtocol(client, req);
	} else {
		UPDATE_TRACE_POINT();
		req->halfCloseAppConnection = false;
		sendHeaderToAppWithHttpProtocol(client, req);
	}

	UPDATE_TRACE_POINT();
	if (!req->ended()) {
		if (req->appSink.acceptingInput()) {
			UPDATE_TRACE_POINT();
			sendBodyToApp(client, req);
			if (!req->ended()) {
				req->appSource.startReading();
			}
		} else if (req->appSink.mayAcceptInputLater()) {
			UPDATE_TRACE_POINT();
			SKC_TRACE(client, 3, "Waiting for appSink channel to become "
				"idle before sending body to application");
			req->appSink.setConsumedCallback(sendBodyToAppWhenAppSinkIdle);
			req->appSource.startReading();
		} else {
			// Either we're done feeding to req->appSink, or req->appSink.feed()
			// encountered an error while writing to the application socket.
			// But we don't care about either scenarios; we just care that
			// ForwardResponse.cpp will now forward the response data and end the
			// request when it's done.
			UPDATE_TRACE_POINT();
			assert(req->appSink.ended() || req->appSink.hasError());
			logAppSocketWriteError(client, req->appSink.getErrcode());
			req->state = Request::WAITING_FOR_APP_OUTPUT;
			req->appSource.startReading();
		}
	}
}