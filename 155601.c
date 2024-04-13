ns_client_newdbversion(ns_client_t *client, unsigned int n) {
	unsigned int i;
	ns_dbversion_t *dbversion = NULL;

	for (i = 0; i < n; i++) {
		dbversion = isc_mem_get(client->mctx, sizeof(*dbversion));
		*dbversion = (ns_dbversion_t){ 0 };
		ISC_LIST_INITANDAPPEND(client->query.freeversions, dbversion,
				       link);
	}

	return (ISC_R_SUCCESS);
}