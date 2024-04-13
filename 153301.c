dns_message_gettempoffsets(dns_message_t *msg, dns_offsets_t **item) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(item != NULL && *item == NULL);

	*item = newoffsets(msg);
	if (*item == NULL)
		return (ISC_R_NOMEMORY);

	return (ISC_R_SUCCESS);
}