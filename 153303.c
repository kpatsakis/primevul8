dns_message_gettimeadjust(dns_message_t *msg) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	return (msg->timeadjust);
}