PgSocket *accept_client(int sock, bool is_unix)
{
	bool res;
	PgSocket *client;

	/* get free PgSocket */
	client = slab_alloc(client_cache);
	if (!client) {
		log_warning("cannot allocate client struct");
		safe_close(sock);
		return NULL;
	}

	client->connect_time = client->request_time = get_cached_time();
	client->query_start = 0;

	/* FIXME: take local and remote address from pool_accept() */
	fill_remote_addr(client, sock, is_unix);
	fill_local_addr(client, sock, is_unix);

	change_client_state(client, CL_LOGIN);

	res = sbuf_accept(&client->sbuf, sock, is_unix);
	if (!res) {
		if (cf_log_connections)
			slog_debug(client, "failed connection attempt");
		return NULL;
	}

	return client;
}