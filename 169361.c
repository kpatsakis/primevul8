static void connect_server(struct PgSocket *server, const struct sockaddr *sa, int salen)
{
	bool res;

	/* fill remote_addr */
	memset(&server->remote_addr, 0, sizeof(server->remote_addr));
	if (sa->sa_family == AF_UNIX) {
		pga_set(&server->remote_addr, AF_UNIX, server->pool->db->port);
	} else {
		pga_copy(&server->remote_addr, sa);
	}

	if (cf_log_connections)
		slog_info(server, "new connection to server");

	/* start connecting */
	res = sbuf_connect(&server->sbuf, sa, salen,
			   cf_server_connect_timeout / USEC);
	if (!res)
		log_noise("failed to launch new connection");
}