dns_message_puttemprdatalist(dns_message_t *msg, dns_rdatalist_t **item) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(item != NULL && *item != NULL);

	releaserdatalist(msg, *item);
	*item = NULL;
}