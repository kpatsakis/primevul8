static enum_func_status
MYSQLND_METHOD(mysqlnd_protocol, send_command)(
		MYSQLND_PROTOCOL_PAYLOAD_DECODER_FACTORY * payload_decoder_factory,
		const enum php_mysqlnd_server_command command,
		const zend_uchar * const arg, const size_t arg_len,
		const zend_bool silent,

		struct st_mysqlnd_connection_state * connection_state,
		MYSQLND_ERROR_INFO * error_info,
		MYSQLND_UPSERT_STATUS * upsert_status,
		MYSQLND_STATS * stats,
		func_mysqlnd_conn_data__send_close send_close,
		void * send_close_ctx)
{
	enum_func_status ret = PASS;
	MYSQLND_PACKET_COMMAND cmd_packet;
	enum mysqlnd_connection_state state;
	DBG_ENTER("mysqlnd_protocol::send_command");
	DBG_INF_FMT("command=%s silent=%u", mysqlnd_command_to_text[command], silent);
	DBG_INF_FMT("server_status=%u", UPSERT_STATUS_GET_SERVER_STATUS(upsert_status));
	DBG_INF_FMT("sending %u bytes", arg_len + 1); /* + 1 is for the command */
	state = connection_state->m->get(connection_state);

	switch (state) {
		case CONN_READY:
			break;
		case CONN_QUIT_SENT:
			SET_CLIENT_ERROR(error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);
			DBG_ERR("Server is gone");
			DBG_RETURN(FAIL);
		default:
			SET_CLIENT_ERROR(error_info, CR_COMMANDS_OUT_OF_SYNC, UNKNOWN_SQLSTATE, mysqlnd_out_of_sync);
			DBG_ERR_FMT("Command out of sync. State=%u", state);
			DBG_RETURN(FAIL);
	}

	UPSERT_STATUS_SET_AFFECTED_ROWS_TO_ERROR(upsert_status);
	SET_EMPTY_ERROR(error_info);

	payload_decoder_factory->m.init_command_packet(&cmd_packet);

	cmd_packet.command = command;
	if (arg && arg_len) {
		cmd_packet.argument.s = (char *) arg;
		cmd_packet.argument.l = arg_len;
	}

	MYSQLND_INC_CONN_STATISTIC(stats, STAT_COM_QUIT + command - 1 /* because of COM_SLEEP */ );

	if (! PACKET_WRITE(payload_decoder_factory->conn, &cmd_packet)) {
		if (!silent && error_info->error_no != CR_SERVER_GONE_ERROR) {
			DBG_ERR_FMT("Error while sending %s packet", mysqlnd_command_to_text[command]);
			php_error(E_WARNING, "Error while sending %s packet. PID=%d", mysqlnd_command_to_text[command], getpid());
		}
		connection_state->m->set(connection_state, CONN_QUIT_SENT);
		send_close(send_close_ctx);
		DBG_ERR("Server is gone");
		ret = FAIL;
	}
	PACKET_FREE(&cmd_packet);
	DBG_RETURN(ret);