bool finish_client_login(PgSocket *client)
{
	switch (client->state) {
	case CL_LOGIN:
		change_client_state(client, CL_ACTIVE);
	case CL_ACTIVE:
		break;
	default:
		fatal("bad client state");
	}

	/* check if we know server signature */
	if (!client->pool->welcome_msg_ready) {
		log_debug("finish_client_login: no welcome message, pause");
		client->wait_for_welcome = 1;
		pause_client(client);
		if (cf_pause_mode == P_NONE)
			launch_new_connection(client->pool);
		return false;
	}
	client->wait_for_welcome = 0;

	/* send the message */
	if (!welcome_client(client))
		return false;

	slog_debug(client, "logged in");

	return true;
}