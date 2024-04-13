ns_client_drop(ns_client_t *client, isc_result_t result) {
	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(client->state == NS_CLIENTSTATE_WORKING ||
		client->state == NS_CLIENTSTATE_RECURSING);

	CTRACE("drop");
	if (result != ISC_R_SUCCESS) {
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "request failed: %s", isc_result_totext(result));
	}
}