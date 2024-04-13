dns_message_renderreserve(dns_message_t *msg, unsigned int space) {
	isc_region_t r;

	REQUIRE(DNS_MESSAGE_VALID(msg));

	if (msg->buffer != NULL) {
		isc_buffer_availableregion(msg->buffer, &r);
		if (r.length < (space + msg->reserved))
			return (ISC_R_NOSPACE);
	}

	msg->reserved += space;

	return (ISC_R_SUCCESS);
}