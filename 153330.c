dns_message_puttempname(dns_message_t *msg, dns_name_t **item) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(item != NULL && *item != NULL);

	if (dns_name_dynamic(*item))
		dns_name_free(*item, msg->mctx);
	isc_mempool_put(msg->namepool, *item);
	*item = NULL;
}