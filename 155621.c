ns_client_qnamereplace(ns_client_t *client, dns_name_t *name) {
	LOCK(&client->query.fetchlock);
	if (client->query.restarts > 0) {
		/*
		 * client->query.qname was dynamically allocated.
		 */
		dns_message_puttempname(client->message, &client->query.qname);
	}
	client->query.qname = name;
	client->query.attributes &= ~NS_QUERYATTR_REDIRECT;
	UNLOCK(&client->query.fetchlock);
}