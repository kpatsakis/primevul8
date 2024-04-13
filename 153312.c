dns_message_gettempname(dns_message_t *msg, dns_name_t **item) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(item != NULL && *item == NULL);

	*item = isc_mempool_get(msg->namepool);
	if (*item == NULL)
		return (ISC_R_NOMEMORY);
	dns_name_init(*item, NULL);

	return (ISC_R_SUCCESS);
}