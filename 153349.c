dns_message_getrawmessage(dns_message_t *msg) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	return (&msg->saved);
}