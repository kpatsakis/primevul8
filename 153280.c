dns_message_gettemprdatalist(dns_message_t *msg, dns_rdatalist_t **item) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(item != NULL && *item == NULL);

	*item = newrdatalist(msg);
	if (*item == NULL)
		return (ISC_R_NOMEMORY);

	return (ISC_R_SUCCESS);
}