ns_client_keepname(ns_client_t *client, dns_name_t *name, isc_buffer_t *dbuf) {
	isc_region_t r;

	CTRACE("ns_client_keepname");

	/*%
	 * 'name' is using space in 'dbuf', but 'dbuf' has not yet been
	 * adjusted to take account of that.  We do the adjustment.
	 */
	REQUIRE((client->query.attributes & NS_QUERYATTR_NAMEBUFUSED) != 0);

	dns_name_toregion(name, &r);
	isc_buffer_add(dbuf, r.length);
	dns_name_setbuffer(name, NULL);
	client->query.attributes &= ~NS_QUERYATTR_NAMEBUFUSED;
}