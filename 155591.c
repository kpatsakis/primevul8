ns__client_tcpconn(isc_nmhandle_t *handle, isc_result_t result, void *arg) {
	ns_interface_t *ifp = (ns_interface_t *)arg;
	dns_aclenv_t *env = ns_interfacemgr_getaclenv(ifp->mgr);
	ns_server_t *sctx = ns_interfacemgr_getserver(ifp->mgr);
	unsigned int tcpquota;
	isc_sockaddr_t peeraddr;
	isc_netaddr_t netaddr;
	int match;

	if (result != ISC_R_SUCCESS) {
		return (result);
	}

	if (handle != NULL) {
		peeraddr = isc_nmhandle_peeraddr(handle);
		isc_netaddr_fromsockaddr(&netaddr, &peeraddr);

		if (sctx->blackholeacl != NULL &&
		    (dns_acl_match(&netaddr, NULL, sctx->blackholeacl, env,
				   &match, NULL) == ISC_R_SUCCESS) &&
		    match > 0)
		{
			return (ISC_R_CONNREFUSED);
		}
	}

	tcpquota = isc_quota_getused(&sctx->tcpquota);
	ns_stats_update_if_greater(sctx->nsstats, ns_statscounter_tcphighwater,
				   tcpquota);

	return (ISC_R_SUCCESS);
}