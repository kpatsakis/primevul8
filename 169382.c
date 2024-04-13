static bool reset_on_release(PgSocket *server)
{
	bool res;
	
	Assert(server->state == SV_TESTED);

	slog_debug(server, "Resetting: %s", cf_server_reset_query);
	SEND_generic(res, server, 'Q', "s", cf_server_reset_query);
	if (!res)
		disconnect_server(server, false, "reset query failed");
	return res;
}