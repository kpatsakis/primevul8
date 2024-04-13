dns_message_gettsig(dns_message_t *msg, dns_name_t **owner) {

	/*
	 * Get the TSIG record and owner for 'msg'.
	 */

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(owner == NULL || *owner == NULL);

	if (owner != NULL)
		*owner = msg->tsigname;
	return (msg->tsig);
}