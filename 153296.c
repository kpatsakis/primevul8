dns_message_gettemprdata(dns_message_t *msg, dns_rdata_t **item) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(item != NULL && *item == NULL);

	*item = newrdata(msg);
	if (*item == NULL)
		return (ISC_R_NOMEMORY);

	return (ISC_R_SUCCESS);
}