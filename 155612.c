ns_client_releasename(ns_client_t *client, dns_name_t **namep) {
	/*%
	 * 'name' is no longer needed.  Return it to our pool of temporary
	 * names.  If it is using a name buffer, relinquish its exclusive
	 * rights on the buffer.
	 */

	CTRACE("ns_client_releasename");
	client->query.attributes &= ~NS_QUERYATTR_NAMEBUFUSED;
	dns_message_puttempname(client->message, namep);
	CTRACE("ns_client_releasename: done");
}