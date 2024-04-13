ns_client_putrdataset(ns_client_t *client, dns_rdataset_t **rdatasetp) {
	dns_rdataset_t *rdataset;

	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(rdatasetp != NULL);

	rdataset = *rdatasetp;

	if (rdataset != NULL) {
		if (dns_rdataset_isassociated(rdataset)) {
			dns_rdataset_disassociate(rdataset);
		}
		dns_message_puttemprdataset(client->message, rdatasetp);
	}
}