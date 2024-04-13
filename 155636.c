ns_client_getnamebuf(ns_client_t *client) {
	isc_buffer_t *dbuf;
	isc_region_t r;

	CTRACE("ns_client_getnamebuf");

	/*%
	 * Return a name buffer with space for a maximal name, allocating
	 * a new one if necessary.
	 */
	if (ISC_LIST_EMPTY(client->query.namebufs)) {
		ns_client_newnamebuf(client);
	}

	dbuf = ISC_LIST_TAIL(client->query.namebufs);
	INSIST(dbuf != NULL);
	isc_buffer_availableregion(dbuf, &r);
	if (r.length < DNS_NAME_MAXWIRE) {
		ns_client_newnamebuf(client);
		dbuf = ISC_LIST_TAIL(client->query.namebufs);
		isc_buffer_availableregion(dbuf, &r);
		INSIST(r.length >= 255);
	}
	CTRACE("ns_client_getnamebuf: done");
	return (dbuf);
}