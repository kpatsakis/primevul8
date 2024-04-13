dns_message_destroy(dns_message_t **msgp) {
	dns_message_t *msg;

	REQUIRE(msgp != NULL);
	REQUIRE(DNS_MESSAGE_VALID(*msgp));

	msg = *msgp;
	*msgp = NULL;

	msgreset(msg, true);
	isc_mempool_destroy(&msg->namepool);
	isc_mempool_destroy(&msg->rdspool);
	msg->magic = 0;
	isc_mem_putanddetach(&msg->mctx, msg, sizeof(dns_message_t));
}