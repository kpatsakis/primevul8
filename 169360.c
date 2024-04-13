static void dns_connect(struct PgSocket *server)
{
	struct sockaddr_un sa_un;
	struct sockaddr_in sa_in;
	struct sockaddr_in6 sa_in6;
	struct sockaddr *sa;
	struct PgDatabase *db = server->pool->db;
	const char *host = db->host;
	const char *unix_dir;
	int sa_len;

	if (!host || host[0] == '/') {
		slog_noise(server, "unix socket: %s", sa_un.sun_path);
		memset(&sa_un, 0, sizeof(sa_un));
		sa_un.sun_family = AF_UNIX;
		unix_dir = host ? host : cf_unix_socket_dir;
		if (!unix_dir || !*unix_dir) {
			log_error("Unix socket dir not configured: %s", db->name);
			disconnect_server(server, false, "cannot connect");
			return;
		}
		snprintf(sa_un.sun_path, sizeof(sa_un.sun_path),
			 "%s/.s.PGSQL.%d", unix_dir, db->port);
		sa = (struct sockaddr *)&sa_un;
		sa_len = sizeof(sa_un);
	} else if (strchr(host, ':')) {  // assume IPv6 address on any : in addr
		slog_noise(server, "inet6 socket: %s", db->host);
		memset(&sa_in6, 0, sizeof(sa_in6));
		sa_in6.sin6_family = AF_INET6;
		inet_pton(AF_INET6, db->host, (void *) sa_in6.sin6_addr.s6_addr);
		sa_in6.sin6_port = htons(db->port);
		sa = (struct sockaddr *)&sa_in6;
		sa_len = sizeof(sa_in6);
	} else if (host[0] >= '0' && host[0] <= '9') { // else try IPv4
		slog_noise(server, "inet socket: %s", db->host);
		memset(&sa_in, 0, sizeof(sa_in));
		sa_in.sin_family = AF_INET;
		sa_in.sin_addr.s_addr = inet_addr(db->host);
		sa_in.sin_port = htons(db->port);
		sa = (struct sockaddr *)&sa_in;
		sa_len = sizeof(sa_in);
	} else {
		struct DNSToken *tk;
		slog_noise(server, "dns socket: %s", db->host);
		/* launch dns lookup */
		tk = adns_resolve(adns, db->host, dns_callback, server);
		if (tk)
			server->dns_token = tk;
		return;
	}

	connect_server(server, sa, sa_len);
}