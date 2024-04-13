dns_message_puttemprdata(dns_message_t *msg, dns_rdata_t **item) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(item != NULL && *item != NULL);

	releaserdata(msg, *item);
	*item = NULL;
}