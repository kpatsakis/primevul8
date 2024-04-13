void disconnect_server(PgSocket *server, bool notify, const char *reason, ...)
{
	PgPool *pool = server->pool;
	PgSocket *client;
	static const uint8_t pkt_term[] = {'X', 0,0,0,4};
	int send_term = 1;
	usec_t now = get_cached_time();
	char buf[128];
	va_list ap;

	va_start(ap, reason);
	vsnprintf(buf, sizeof(buf), reason, ap);
	va_end(ap);
	reason = buf;

	if (cf_log_disconnections)
		slog_info(server, "closing because: %s (age=%" PRIu64 ")", reason,
			  (now - server->connect_time) / USEC);

	switch (server->state) {
	case SV_ACTIVE:
		client = server->link;
		if (client) {
			client->link = NULL;
			server->link = NULL;
			disconnect_client(client, true, "%s", reason);
		}
		break;
	case SV_TESTED:
	case SV_USED:
	case SV_IDLE:
		break;
	case SV_LOGIN:
		/*
		 * usually disconnect means problems in startup phase,
		 * except when sending cancel packet
		 */
		if (!server->ready)
			pool->last_connect_failed = 1;
		else
			send_term = 0;
		break;
	default:
		fatal("disconnect_server: bad server state (%d)", server->state);
	}

	Assert(server->link == NULL);

	/* notify server and close connection */
	if (send_term && notify) {
		if (!sbuf_answer(&server->sbuf, pkt_term, sizeof(pkt_term)))
			/* ignore result */
			notify = false;
	}

	if (server->dns_token) {
		adns_cancel(adns, server->dns_token);
		server->dns_token = NULL;
	}

	change_server_state(server, SV_JUSTFREE);
	if (!sbuf_close(&server->sbuf))
		log_noise("sbuf_close failed, retry later");
}