dns_message_puttemprdataset(dns_message_t *msg, dns_rdataset_t **item) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(item != NULL && *item != NULL);

	REQUIRE(!dns_rdataset_isassociated(*item));
	isc_mempool_put(msg->rdspool, *item);
	*item = NULL;
}