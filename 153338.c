dns_message_setsig0key(dns_message_t *msg, dst_key_t *key) {
	isc_region_t r;
	unsigned int x;
	isc_result_t result;

	/*
	 * Set the SIG(0) key for 'msg'
	 */

	/*
	 * The space required for an SIG(0) record is:
	 *
	 *	1 byte for the name
	 *	2 bytes for the type
	 *	2 bytes for the class
	 *	4 bytes for the ttl
	 *	2 bytes for the type covered
	 *	1 byte for the algorithm
	 *	1 bytes for the labels
	 *	4 bytes for the original ttl
	 *	4 bytes for the signature expiration
	 *	4 bytes for the signature inception
	 *	2 bytes for the key tag
	 *	n bytes for the signer's name
	 *	x bytes for the signature
	 * ---------------------------------
	 *     27 + n + x bytes
	 */
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(msg->from_to_wire == DNS_MESSAGE_INTENTRENDER);
	REQUIRE(msg->state == DNS_SECTION_ANY);

	if (key != NULL) {
		REQUIRE(msg->sig0key == NULL && msg->tsigkey == NULL);
		dns_name_toregion(dst_key_name(key), &r);
		result = dst_key_sigsize(key, &x);
		if (result != ISC_R_SUCCESS) {
			msg->sig_reserved = 0;
			return (result);
		}
		msg->sig_reserved = 27 + r.length + x;
		result = dns_message_renderreserve(msg, msg->sig_reserved);
		if (result != ISC_R_SUCCESS) {
			msg->sig_reserved = 0;
			return (result);
		}
		msg->sig0key = key;
	}
	return (ISC_R_SUCCESS);
}