dns_message_gettsigkey(dns_message_t *msg) {

	/*
	 * Get the TSIG key for 'msg'
	 */

	REQUIRE(DNS_MESSAGE_VALID(msg));

	return (msg->tsigkey);
}