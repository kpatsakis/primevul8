static void dns_callback(void *arg, const struct sockaddr *sa, int salen)
{
	struct PgSocket *server = arg;
	struct PgDatabase *db = server->pool->db;
	struct sockaddr_in sa_in;
	struct sockaddr_in6 sa_in6;

	server->dns_token = NULL;

	if (!sa) {
		disconnect_server(server, true, "server dns lookup failed");
		return;
	} else if (sa->sa_family == AF_INET) {
		char buf[64];
		memcpy(&sa_in, sa, sizeof(sa_in));
		sa_in.sin_port = htons(db->port);
		sa = (struct sockaddr *)&sa_in;
		salen = sizeof(sa_in);
		slog_debug(server, "dns_callback: inet4: %s",
			   sa2str(sa, buf, sizeof(buf)));
	} else if (sa->sa_family == AF_INET6) {
		char buf[64];
		memcpy(&sa_in6, sa, sizeof(sa_in6));
		sa_in6.sin6_port = htons(db->port);
		sa = (struct sockaddr *)&sa_in6;
		salen = sizeof(sa_in6);
		slog_debug(server, "dns_callback: inet6: %s",
			   sa2str(sa, buf, sizeof(buf)));
	} else {
		disconnect_server(server, true, "unknown address family: %d", sa->sa_family);
		return;
	}

	connect_server(server, sa, salen);
}