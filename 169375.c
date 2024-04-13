void activate_client(PgSocket *client)
{
	Assert(client->state == CL_WAITING);

	slog_debug(client, "activate_client");
	change_client_state(client, CL_ACTIVE);
	sbuf_continue(&client->sbuf);
}