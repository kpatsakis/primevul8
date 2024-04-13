ns_client_checkaclsilent(ns_client_t *client, isc_netaddr_t *netaddr,
			 dns_acl_t *acl, bool default_allow) {
	isc_result_t result;
	dns_aclenv_t *env =
		ns_interfacemgr_getaclenv(client->manager->interface->mgr);
	isc_netaddr_t tmpnetaddr;
	int match;

	if (acl == NULL) {
		if (default_allow) {
			goto allow;
		} else {
			goto deny;
		}
	}

	if (netaddr == NULL) {
		isc_netaddr_fromsockaddr(&tmpnetaddr, &client->peeraddr);
		netaddr = &tmpnetaddr;
	}

	result = dns_acl_match(netaddr, client->signer, acl, env, &match, NULL);
	if (result != ISC_R_SUCCESS) {
		goto deny; /* Internal error, already logged. */
	}

	if (match > 0) {
		goto allow;
	}
	goto deny; /* Negative match or no match. */

allow:
	return (ISC_R_SUCCESS);

deny:
	return (DNS_R_REFUSED);
}