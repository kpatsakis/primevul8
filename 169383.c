bool use_server_socket(int fd, PgAddr *addr,
		       const char *dbname, const char *username,
		       uint64_t ckey, int oldfd, int linkfd,
		       const char *client_enc, const char *std_string,
		       const char *datestyle, const char *timezone)
{
	PgDatabase *db = find_database(dbname);
	PgUser *user;
	PgPool *pool;
	PgSocket *server;
	PktBuf tmp;
	bool res;
	
	/* if the database not found, it's an auto database -> registering... */
	if (!db) {
		db = register_auto_database(dbname);
		if (!db)
			return true;
	}

	if (db->forced_user)
		user = db->forced_user;
	else
		user = find_user(username);

	pool = get_pool(db, user);
	if (!pool)
		return false;

	server = slab_alloc(server_cache);
	if (!server)
		return false;

	res = sbuf_accept(&server->sbuf, fd, pga_is_unix(addr));
	if (!res)
		return false;

	server->suspended = 1;
	server->pool = pool;
	server->auth_user = user;
	server->connect_time = server->request_time = get_cached_time();
	server->query_start = 0;

	fill_remote_addr(server, fd, pga_is_unix(addr));
	fill_local_addr(server, fd, pga_is_unix(addr));

	if (linkfd) {
		server->ready = 0;
		change_server_state(server, SV_ACTIVE);
	} else {
		server->ready = 1;
		change_server_state(server, SV_IDLE);
	}

	/* store old cancel key */
	pktbuf_static(&tmp, server->cancel_key, 8);
	pktbuf_put_uint64(&tmp, ckey);

	/* store old fds */
	server->tmp_sk_oldfd = oldfd;
	server->tmp_sk_linkfd = linkfd;

	varcache_set(&server->vars, "client_encoding", client_enc);
	varcache_set(&server->vars, "standard_conforming_strings", std_string);
	varcache_set(&server->vars, "datestyle", datestyle);
	varcache_set(&server->vars, "timezone", timezone);

	return true;
}