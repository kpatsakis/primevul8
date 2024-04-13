ns_client_newrdataset(ns_client_t *client) {
	dns_rdataset_t *rdataset;
	isc_result_t result;

	REQUIRE(NS_CLIENT_VALID(client));

	rdataset = NULL;
	result = dns_message_gettemprdataset(client->message, &rdataset);
	if (result != ISC_R_SUCCESS) {
		return (NULL);
	}

	return (rdataset);
}