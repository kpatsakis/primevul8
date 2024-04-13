client_sendpkg(ns_client_t *client, isc_buffer_t *buffer) {
	isc_region_t r;

	REQUIRE(client->sendhandle == NULL);

	isc_buffer_usedregion(buffer, &r);
	isc_nmhandle_attach(client->handle, &client->sendhandle);
	isc_nm_send(client->handle, &r, client_senddone, client);
}