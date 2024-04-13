static void pause_client(PgSocket *client)
{
	Assert(client->state == CL_ACTIVE);

	slog_debug(client, "pause_client");
	change_client_state(client, CL_WAITING);
	if (!sbuf_pause(&client->sbuf))
		disconnect_client(client, true, "pause failed");
}