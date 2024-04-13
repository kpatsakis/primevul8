dns_message_renderbegin(dns_message_t *msg, dns_compress_t *cctx,
			isc_buffer_t *buffer)
{
	isc_region_t r;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(buffer != NULL);
	REQUIRE(msg->buffer == NULL);
	REQUIRE(msg->from_to_wire == DNS_MESSAGE_INTENTRENDER);

	msg->cctx = cctx;

	/*
	 * Erase the contents of this buffer.
	 */
	isc_buffer_clear(buffer);

	/*
	 * Make certain there is enough for at least the header in this
	 * buffer.
	 */
	isc_buffer_availableregion(buffer, &r);
	if (r.length < DNS_MESSAGE_HEADERLEN)
		return (ISC_R_NOSPACE);

	if (r.length - DNS_MESSAGE_HEADERLEN < msg->reserved)
		return (ISC_R_NOSPACE);

	/*
	 * Reserve enough space for the header in this buffer.
	 */
	isc_buffer_add(buffer, DNS_MESSAGE_HEADERLEN);

	msg->buffer = buffer;

	return (ISC_R_SUCCESS);
}