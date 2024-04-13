dns_message_gettemprdataset(dns_message_t *msg, dns_rdataset_t **item) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(item != NULL && *item == NULL);

	*item = isc_mempool_get(msg->rdspool);
	if (*item == NULL)
		return (ISC_R_NOMEMORY);

	dns_rdataset_init(*item);

	return (ISC_R_SUCCESS);
}