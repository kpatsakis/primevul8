dns_message_setquerytsig(dns_message_t *msg, isc_buffer_t *querytsig) {
	dns_rdata_t *rdata = NULL;
	dns_rdatalist_t *list = NULL;
	dns_rdataset_t *set = NULL;
	isc_buffer_t *buf = NULL;
	isc_region_t r;
	isc_result_t result;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(msg->querytsig == NULL);

	if (querytsig == NULL)
		return (ISC_R_SUCCESS);

	result = dns_message_gettemprdata(msg, &rdata);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	result = dns_message_gettemprdatalist(msg, &list);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	result = dns_message_gettemprdataset(msg, &set);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	isc_buffer_usedregion(querytsig, &r);
	result = isc_buffer_allocate(msg->mctx, &buf, r.length);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	isc_buffer_putmem(buf, r.base, r.length);
	isc_buffer_usedregion(buf, &r);
	dns_rdata_init(rdata);
	dns_rdata_fromregion(rdata, dns_rdataclass_any, dns_rdatatype_tsig, &r);
	dns_message_takebuffer(msg, &buf);
	ISC_LIST_APPEND(list->rdata, rdata, link);
	result = dns_rdatalist_tordataset(list, set);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	msg->querytsig = set;

	return (result);

 cleanup:
	if (rdata != NULL)
		dns_message_puttemprdata(msg, &rdata);
	if (list != NULL)
		dns_message_puttemprdatalist(msg, &list);
	if (set != NULL)
		dns_message_puttemprdataset(msg, &set);
	return (ISC_R_NOMEMORY);
}