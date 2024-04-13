Controller::halfCloseAppSink(Client *client, Request *req) {
	P_ASSERT_EQ(req->state, Request::WAITING_FOR_APP_OUTPUT);
	if (req->halfCloseAppConnection) {
		SKC_TRACE(client, 3, "Half-closing application socket with SHUT_WR");
		::shutdown(req->session->fd(), SHUT_WR);
	}
}