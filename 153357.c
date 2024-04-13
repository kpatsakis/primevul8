dns_message_setopt(dns_message_t *msg, dns_rdataset_t *opt) {
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;

	/*
	 * Set the OPT record for 'msg'.
	 */

	/*
	 * The space required for an OPT record is:
	 *
	 *	1 byte for the name
	 *	2 bytes for the type
	 *	2 bytes for the class
	 *	4 bytes for the ttl
	 *	2 bytes for the rdata length
	 * ---------------------------------
	 *     11 bytes
	 *
	 * plus the length of the rdata.
	 */

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(opt->type == dns_rdatatype_opt);
	REQUIRE(msg->from_to_wire == DNS_MESSAGE_INTENTRENDER);
	REQUIRE(msg->state == DNS_SECTION_ANY);

	msgresetopt(msg);

	result = dns_rdataset_first(opt);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	dns_rdataset_current(opt, &rdata);
	msg->opt_reserved = 11 + rdata.length;
	result = dns_message_renderreserve(msg, msg->opt_reserved);
	if (result != ISC_R_SUCCESS) {
		msg->opt_reserved = 0;
		goto cleanup;
	}

	msg->opt = opt;

	return (ISC_R_SUCCESS);

 cleanup:
	dns_rdataset_disassociate(opt);
	dns_message_puttemprdataset(msg, &opt);
	return (result);
}