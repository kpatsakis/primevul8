ns_client_sendraw(ns_client_t *client, dns_message_t *message) {
	isc_result_t result;
	unsigned char *data;
	isc_buffer_t buffer;
	isc_region_t r;
	isc_region_t *mr;

	REQUIRE(NS_CLIENT_VALID(client));

	CTRACE("sendraw");

	mr = dns_message_getrawmessage(message);
	if (mr == NULL) {
		result = ISC_R_UNEXPECTEDEND;
		goto done;
	}

	client_allocsendbuf(client, &buffer, &data);

	if (mr->length > isc_buffer_length(&buffer)) {
		result = ISC_R_NOSPACE;
		goto done;
	}

	/*
	 * Copy message to buffer and fixup id.
	 */
	isc_buffer_availableregion(&buffer, &r);
	result = isc_buffer_copyregion(&buffer, mr);
	if (result != ISC_R_SUCCESS) {
		goto done;
	}
	r.base[0] = (client->message->id >> 8) & 0xff;
	r.base[1] = client->message->id & 0xff;

#ifdef HAVE_DNSTAP
	if (client->view != NULL) {
		bool tcp = TCP_CLIENT(client);
		dns_dtmsgtype_t dtmsgtype;
		if (client->message->opcode == dns_opcode_update) {
			dtmsgtype = DNS_DTTYPE_UR;
		} else if ((client->message->flags & DNS_MESSAGEFLAG_RD) != 0) {
			dtmsgtype = DNS_DTTYPE_CR;
		} else {
			dtmsgtype = DNS_DTTYPE_AR;
		}
		dns_dt_send(client->view, dtmsgtype, &client->peeraddr,
			    &client->destsockaddr, tcp, NULL,
			    &client->requesttime, NULL, &buffer);
	}
#endif

	client_sendpkg(client, &buffer);

	return;
done:
	if (client->tcpbuf != NULL) {
		isc_mem_put(client->mctx, client->tcpbuf,
			    NS_CLIENT_TCP_BUFFER_SIZE);
		client->tcpbuf = NULL;
	}

	ns_client_drop(client, result);
}