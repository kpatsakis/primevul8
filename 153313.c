dns_message_settsigkey(dns_message_t *msg, dns_tsigkey_t *key) {
	isc_result_t result;

	/*
	 * Set the TSIG key for 'msg'
	 */

	REQUIRE(DNS_MESSAGE_VALID(msg));

	if (key == NULL && msg->tsigkey != NULL) {
		if (msg->sig_reserved != 0) {
			dns_message_renderrelease(msg, msg->sig_reserved);
			msg->sig_reserved = 0;
		}
		dns_tsigkey_detach(&msg->tsigkey);
	}
	if (key != NULL) {
		REQUIRE(msg->tsigkey == NULL && msg->sig0key == NULL);
		dns_tsigkey_attach(key, &msg->tsigkey);
		if (msg->from_to_wire == DNS_MESSAGE_INTENTRENDER) {
			msg->sig_reserved = spacefortsig(msg->tsigkey, 0);
			result = dns_message_renderreserve(msg,
							   msg->sig_reserved);
			if (result != ISC_R_SUCCESS) {
				dns_tsigkey_detach(&msg->tsigkey);
				msg->sig_reserved = 0;
				return (result);
			}
		}
	}
	return (ISC_R_SUCCESS);
}