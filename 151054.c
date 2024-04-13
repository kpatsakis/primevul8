Controller::sendBodyToApp(Client *client, Request *req) {
	TRACE_POINT();
	assert(req->appSink.acceptingInput());
	#ifdef DEBUG_CC_EVENT_LOOP_BLOCKING
		req->timeOnRequestHeaderSent = ev_now(getLoop());
		reportLargeTimeDiff(client,
			"ApplicationPool get until headers sent",
			req->timeBeforeAccessingApplicationPool,
			req->timeOnRequestHeaderSent);
	#endif
	if (req->hasBody() || req->upgraded()) {
		// onRequestBody() will take care of forwarding
		// the request body to the app.
		SKC_TRACE(client, 2, "Sending body to application");
		req->state = Request::FORWARDING_BODY_TO_APP;
		startBodyChannel(client, req);
	} else {
		// Our task is done. ForwardResponse.cpp will take
		// care of ending the request, once all response
		// data is forwarded.
		SKC_TRACE(client, 2, "No body to send to application");
		req->state = Request::WAITING_FOR_APP_OUTPUT;
		halfCloseAppSink(client, req);
	}
}