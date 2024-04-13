dns_message_getsig0(dns_message_t *msg, dns_name_t **owner) {

	/*
	 * Get the SIG(0) record for 'msg'.
	 */

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(owner == NULL || *owner == NULL);

	if (msg->sig0 != NULL && owner != NULL) {
		/* If dns_message_getsig0 is called on a rendered message
		 * after the SIG(0) has been applied, we need to return the
		 * root name, not NULL.
		 */
		if (msg->sig0name == NULL)
			*owner = dns_rootname;
		else
			*owner = msg->sig0name;
	}
	return (msg->sig0);
}