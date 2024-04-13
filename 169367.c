bool use_client_socket(int fd, PgAddr *addr,
		       const char *dbname, const char *username,
		       uint64_t ckey, int oldfd, int linkfd,
		       const char *client_enc, const char *std_string,
		       const char *datestyle, const char *timezone)
{
	PgSocket *client;
	PktBuf tmp;

	client = accept_client(fd, pga_is_unix(addr));
	if (client == NULL)
		return false;
	client->suspended = 1;

	if (!set_pool(client, dbname, username))
		return false;

	change_client_state(client, CL_ACTIVE);

	/* store old cancel key */
	pktbuf_static(&tmp, client->cancel_key, 8);
	pktbuf_put_uint64(&tmp, ckey);

	/* store old fds */
	client->tmp_sk_oldfd = oldfd;
	client->tmp_sk_linkfd = linkfd;

	varcache_set(&client->vars, "client_encoding", client_enc);
	varcache_set(&client->vars, "standard_conforming_strings", std_string);
	varcache_set(&client->vars, "datestyle", datestyle);
	varcache_set(&client->vars, "timezone", timezone);

	return true;
}