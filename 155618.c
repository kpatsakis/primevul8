ns_client_findversion(ns_client_t *client, dns_db_t *db) {
	ns_dbversion_t *dbversion;

	for (dbversion = ISC_LIST_HEAD(client->query.activeversions);
	     dbversion != NULL; dbversion = ISC_LIST_NEXT(dbversion, link))
	{
		if (dbversion->db == db) {
			break;
		}
	}

	if (dbversion == NULL) {
		/*
		 * This is a new zone for this query.  Add it to
		 * the active list.
		 */
		dbversion = client_getdbversion(client);
		if (dbversion == NULL) {
			return (NULL);
		}
		dns_db_attach(db, &dbversion->db);
		dns_db_currentversion(db, &dbversion->version);
		dbversion->acl_checked = false;
		dbversion->queryok = false;
		ISC_LIST_APPEND(client->query.activeversions, dbversion, link);
	}

	return (dbversion);
}