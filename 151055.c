Controller::logAppSocketWriteError(Client *client, int errcode) {
	if (errcode == EPIPE) {
		SKC_INFO(client,
			"App socket write error: the application closed the socket prematurely"
			" (Broken pipe; errno=" << errcode << ")");
	} else {
		SKC_INFO(client, "App socket write error: " << ServerKit::getErrorDesc(errcode) <<
			" (errno=" << errcode << ")");
	}
}