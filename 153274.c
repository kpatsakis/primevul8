dns_message_reset(dns_message_t *msg, unsigned int intent) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(intent == DNS_MESSAGE_INTENTPARSE
		|| intent == DNS_MESSAGE_INTENTRENDER);

	msgreset(msg, false);
	msg->from_to_wire = intent;
}