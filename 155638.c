ns_client_sourceip(dns_clientinfo_t *ci, isc_sockaddr_t **addrp) {
	ns_client_t *client = (ns_client_t *)ci->data;

	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(addrp != NULL);

	*addrp = &client->peeraddr;
	return (ISC_R_SUCCESS);
}