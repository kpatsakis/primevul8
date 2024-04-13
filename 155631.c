ns_client_name(ns_client_t *client, char *peerbuf, size_t len) {
	if (client->peeraddr_valid) {
		isc_sockaddr_format(&client->peeraddr, peerbuf,
				    (unsigned int)len);
	} else {
		snprintf(peerbuf, len, "@%p", client);
	}
}