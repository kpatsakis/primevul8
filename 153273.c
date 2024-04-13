dns_message_getsig0key(dns_message_t *msg) {

	/*
	 * Get the SIG(0) key for 'msg'
	 */

	REQUIRE(DNS_MESSAGE_VALID(msg));

	return (msg->sig0key);
}