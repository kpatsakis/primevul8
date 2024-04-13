dns_message_takebuffer(dns_message_t *msg, isc_buffer_t **buffer) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(buffer != NULL);
	REQUIRE(ISC_BUFFER_VALID(*buffer));

	ISC_LIST_APPEND(msg->cleanup, *buffer, link);
	*buffer = NULL;
}