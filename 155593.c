ns__client_reset_cb(void *client0) {
	ns_client_t *client = client0;

	ns_client_log(client, DNS_LOGCATEGORY_SECURITY, NS_LOGMODULE_CLIENT,
		      ISC_LOG_DEBUG(3), "reset client");

	/*
	 * We never started processing this client, possible if we're
	 * shutting down, just exit.
	 */
	if (client->state == NS_CLIENTSTATE_READY) {
		return;
	}

	ns_client_endrequest(client);
	if (client->tcpbuf != NULL) {
		isc_mem_put(client->mctx, client->tcpbuf,
			    NS_CLIENT_TCP_BUFFER_SIZE);
	}

	if (client->keytag != NULL) {
		isc_mem_put(client->mctx, client->keytag, client->keytag_len);
		client->keytag_len = 0;
	}

	client->state = NS_CLIENTSTATE_READY;
	INSIST(client->recursionquota == NULL);
}