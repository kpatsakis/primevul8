/* {{{ php_mysqlnd_cmd_write */
size_t php_mysqlnd_cmd_write(MYSQLND_CONN_DATA * conn, void * _packet)
{
	/* Let's have some space, which we can use, if not enough, we will allocate new buffer */
	MYSQLND_PACKET_COMMAND * packet= (MYSQLND_PACKET_COMMAND *) _packet;
	MYSQLND_ERROR_INFO * error_info = conn->error_info;
	MYSQLND_PFC * pfc = conn->protocol_frame_codec;
	MYSQLND_VIO * vio = conn->vio;
	MYSQLND_STATS * stats = conn->stats;
	MYSQLND_CONNECTION_STATE * connection_state = &conn->state;
	const unsigned int error_reporting = EG(error_reporting);
	size_t sent = 0;

	DBG_ENTER("php_mysqlnd_cmd_write");
	/*
	  Reset packet_no, or we will get bad handshake!
	  Every command starts a new TX and packet numbers are reset to 0.
	*/
	pfc->data->m.reset(pfc, stats, error_info);

	if (error_reporting) {
		EG(error_reporting) = 0;
	}

	MYSQLND_INC_CONN_STATISTIC(stats, STAT_PACKETS_SENT_CMD);

#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	vio->data->m.consume_uneaten_data(vio, packet->command);
#endif

	if (!packet->argument.s || !packet->argument.l) {
		zend_uchar buffer[MYSQLND_HEADER_SIZE + 1];

		int1store(buffer + MYSQLND_HEADER_SIZE, packet->command);
		sent = pfc->data->m.send(pfc, vio, buffer, 1, stats, error_info);
	} else {
		size_t tmp_len = packet->argument.l + 1 + MYSQLND_HEADER_SIZE;
		zend_uchar *tmp, *p;
		tmp = (tmp_len > pfc->cmd_buffer.length)? mnd_emalloc(tmp_len):pfc->cmd_buffer.buffer;
		if (!tmp) {
			goto end;
		}
		p = tmp + MYSQLND_HEADER_SIZE; /* skip the header */

		int1store(p, packet->command);
		p++;

		memcpy(p, packet->argument.s, packet->argument.l);

		sent = pfc->data->m.send(pfc, vio, tmp, tmp_len - MYSQLND_HEADER_SIZE, stats, error_info);
		if (tmp != pfc->cmd_buffer.buffer) {
			MYSQLND_INC_CONN_STATISTIC(stats, STAT_CMD_BUFFER_TOO_SMALL);
			mnd_efree(tmp);
		}
	}
end:
	if (error_reporting) {
		/* restore error reporting */
		EG(error_reporting) = error_reporting;
	}
	if (!sent) {
		SET_CONNECTION_STATE(connection_state, CONN_QUIT_SENT);
	}
	DBG_RETURN(sent);