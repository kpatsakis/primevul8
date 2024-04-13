dns_message_peekheader(isc_buffer_t *source, dns_messageid_t *idp,
		       unsigned int *flagsp)
{
	isc_region_t r;
	isc_buffer_t buffer;
	dns_messageid_t id;
	unsigned int flags;

	REQUIRE(source != NULL);

	buffer = *source;

	isc_buffer_remainingregion(&buffer, &r);
	if (r.length < DNS_MESSAGE_HEADERLEN)
		return (ISC_R_UNEXPECTEDEND);

	id = isc_buffer_getuint16(&buffer);
	flags = isc_buffer_getuint16(&buffer);
	flags &= DNS_MESSAGE_FLAG_MASK;

	if (flagsp != NULL)
		*flagsp = flags;
	if (idp != NULL)
		*idp = id;

	return (ISC_R_SUCCESS);
}