client_getdbversion(ns_client_t *client) {
	ns_dbversion_t *dbversion = NULL;

	if (ISC_LIST_EMPTY(client->query.freeversions)) {
		ns_client_newdbversion(client, 1);
	}
	dbversion = ISC_LIST_HEAD(client->query.freeversions);
	INSIST(dbversion != NULL);
	ISC_LIST_UNLINK(client->query.freeversions, dbversion, link);

	return (dbversion);
}