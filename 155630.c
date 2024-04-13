ns_client_newname(ns_client_t *client, isc_buffer_t *dbuf, isc_buffer_t *nbuf) {
	dns_name_t *name = NULL;
	isc_region_t r;
	isc_result_t result;

	REQUIRE((client->query.attributes & NS_QUERYATTR_NAMEBUFUSED) == 0);

	CTRACE("ns_client_newname");

	result = dns_message_gettempname(client->message, &name);
	if (result != ISC_R_SUCCESS) {
		CTRACE("ns_client_newname: "
		       "dns_message_gettempname failed: done");
		return (NULL);
	}
	isc_buffer_availableregion(dbuf, &r);
	isc_buffer_init(nbuf, r.base, r.length);
	dns_name_setbuffer(name, NULL);
	dns_name_setbuffer(name, nbuf);
	client->query.attributes |= NS_QUERYATTR_NAMEBUFUSED;

	CTRACE("ns_client_newname: done");
	return (name);
}