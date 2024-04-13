dns_message_buildopt(dns_message_t *message, dns_rdataset_t **rdatasetp,
		     unsigned int version, uint16_t udpsize,
		     unsigned int flags, dns_ednsopt_t *ednsopts, size_t count)
{
	dns_rdataset_t *rdataset = NULL;
	dns_rdatalist_t *rdatalist = NULL;
	dns_rdata_t *rdata = NULL;
	isc_result_t result;
	unsigned int len = 0, i;

	REQUIRE(DNS_MESSAGE_VALID(message));
	REQUIRE(rdatasetp != NULL && *rdatasetp == NULL);

	result = dns_message_gettemprdatalist(message, &rdatalist);
	if (result != ISC_R_SUCCESS)
		return (result);
	result = dns_message_gettemprdata(message, &rdata);
	if (result != ISC_R_SUCCESS)
		goto cleanup;
	result = dns_message_gettemprdataset(message, &rdataset);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	rdatalist->type = dns_rdatatype_opt;

	/*
	 * Set Maximum UDP buffer size.
	 */
	rdatalist->rdclass = udpsize;

	/*
	 * Set EXTENDED-RCODE and Z to 0.
	 */
	rdatalist->ttl = (version << 16);
	rdatalist->ttl |= (flags & 0xffff);

	/*
	 * Set EDNS options if applicable
	 */
	if (count != 0U) {
		isc_buffer_t *buf = NULL;
		for (i = 0; i < count; i++)
			len += ednsopts[i].length + 4;

		if (len > 0xffffU) {
			result = ISC_R_NOSPACE;
			goto cleanup;
		}

		result = isc_buffer_allocate(message->mctx, &buf, len);
		if (result != ISC_R_SUCCESS)
			goto cleanup;

		for (i = 0; i < count; i++)  {
			isc_buffer_putuint16(buf, ednsopts[i].code);
			isc_buffer_putuint16(buf, ednsopts[i].length);
			if (ednsopts[i].length != 0) {
				isc_buffer_putmem(buf, ednsopts[i].value,
						  ednsopts[i].length);
			}
		}
		rdata->data = isc_buffer_base(buf);
		rdata->length = len;
		dns_message_takebuffer(message, &buf);
	} else {
		rdata->data = NULL;
		rdata->length = 0;
	}

	rdata->rdclass = rdatalist->rdclass;
	rdata->type = rdatalist->type;
	rdata->flags = 0;

	ISC_LIST_APPEND(rdatalist->rdata, rdata, link);
	result = dns_rdatalist_tordataset(rdatalist, rdataset);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	*rdatasetp = rdataset;
	return (ISC_R_SUCCESS);

 cleanup:
	if (rdata != NULL)
		dns_message_puttemprdata(message, &rdata);
	if (rdataset != NULL)
		dns_message_puttemprdataset(message, &rdataset);
	if (rdatalist != NULL)
		dns_message_puttemprdatalist(message, &rdatalist);
	return (result);
}