client_allocsendbuf(ns_client_t *client, isc_buffer_t *buffer,
		    unsigned char **datap) {
	unsigned char *data;
	uint32_t bufsize;

	REQUIRE(datap != NULL);

	if (TCP_CLIENT(client)) {
		INSIST(client->tcpbuf == NULL);
		client->tcpbuf = isc_mem_get(client->mctx,
					     NS_CLIENT_TCP_BUFFER_SIZE);
		data = client->tcpbuf;
		isc_buffer_init(buffer, data, NS_CLIENT_TCP_BUFFER_SIZE);
	} else {
		data = client->sendbuf;
		if ((client->attributes & NS_CLIENTATTR_HAVECOOKIE) == 0) {
			if (client->view != NULL) {
				bufsize = client->view->nocookieudp;
			} else {
				bufsize = 512;
			}
		} else {
			bufsize = client->udpsize;
		}
		if (bufsize > client->udpsize) {
			bufsize = client->udpsize;
		}
		if (bufsize > NS_CLIENT_SEND_BUFFER_SIZE) {
			bufsize = NS_CLIENT_SEND_BUFFER_SIZE;
		}
		isc_buffer_init(buffer, data, bufsize);
	}
	*datap = data;
}