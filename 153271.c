dns_message_settimeadjust(dns_message_t *msg, int timeadjust) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	msg->timeadjust = timeadjust;
}