dns_message_getquerytsig(dns_message_t *msg, isc_mem_t *mctx,
			 isc_buffer_t **querytsig) {
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	isc_region_t r;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(mctx != NULL);
	REQUIRE(querytsig != NULL && *querytsig == NULL);

	if (msg->tsig == NULL)
		return (ISC_R_SUCCESS);

	result = dns_rdataset_first(msg->tsig);
	if (result != ISC_R_SUCCESS)
		return (result);
	dns_rdataset_current(msg->tsig, &rdata);
	dns_rdata_toregion(&rdata, &r);

	result = isc_buffer_allocate(mctx, querytsig, r.length);
	if (result != ISC_R_SUCCESS)
		return (result);
	isc_buffer_putmem(*querytsig, r.base, r.length);
	return (ISC_R_SUCCESS);
}