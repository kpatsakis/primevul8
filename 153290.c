dns_message_renderchangebuffer(dns_message_t *msg, isc_buffer_t *buffer) {
	isc_region_t r, rn;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(buffer != NULL);
	REQUIRE(msg->buffer != NULL);

	/*
	 * Ensure that the new buffer is empty, and has enough space to
	 * hold the current contents.
	 */
	isc_buffer_clear(buffer);

	isc_buffer_availableregion(buffer, &rn);
	isc_buffer_usedregion(msg->buffer, &r);
	REQUIRE(rn.length > r.length);

	/*
	 * Copy the contents from the old to the new buffer.
	 */
	isc_buffer_add(buffer, r.length);
	memmove(rn.base, r.base, r.length);

	msg->buffer = buffer;

	return (ISC_R_SUCCESS);
}