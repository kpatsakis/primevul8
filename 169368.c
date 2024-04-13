void disconnect_client(PgSocket *client, bool notify, const char *reason, ...)
{
	char buf[128];
	va_list ap;
	usec_t now = get_cached_time();

	va_start(ap, reason);
	vsnprintf(buf, sizeof(buf), reason, ap);
	va_end(ap);
	reason = buf;

	if (cf_log_disconnections)
		slog_info(client, "closing because: %s (age=%" PRIu64 ")", reason,
			  (now - client->connect_time) / USEC);

	switch (client->state) {
	case CL_ACTIVE:
		if (client->link) {
			PgSocket *server = client->link;
			/* ->ready may be set before all is sent */
			if (server->ready && sbuf_is_empty(&server->sbuf)) {
				/* retval does not matter here */
				release_server(server);
			} else {
				server->link = NULL;
				client->link = NULL;
				disconnect_server(server, true, "unclean server");
			}
		}
	case CL_LOGIN:
	case CL_WAITING:
	case CL_CANCEL:
		break;
	default:
		fatal("bad client state in disconnect_client: %d", client->state);
	}

	/* send reason to client */
	if (notify && reason && client->state != CL_CANCEL) {
		/*
		 * don't send Ready pkt here, or client won't notice
		 * closed connection
		 */
		send_pooler_error(client, false, reason);
	}

	change_client_state(client, CL_JUSTFREE);
	if (!sbuf_close(&client->sbuf))
		log_noise("sbuf_close failed, retry later");
}