dns_message_getopt(dns_message_t *msg) {

	/*
	 * Get the OPT record for 'msg'.
	 */

	REQUIRE(DNS_MESSAGE_VALID(msg));

	return (msg->opt);
}