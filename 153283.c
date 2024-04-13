dns_message_renderrelease(dns_message_t *msg, unsigned int space) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(space <= msg->reserved);

	msg->reserved -= space;
}